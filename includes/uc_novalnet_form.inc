<?php
/**
 * Novalnet payment module related file
 * This module is used for real time processing of
 * Novalnet transaction of customers.
 *
 * @category   PHP
 * @package    uc_novalnet
 * @author     Novalnet AG
 * @copyright  Copyright by Novalnet
 * @license    https://www.novalnet.de/payment-plugins/kostenlos/lizenz
 *
 * Script   : uc_novalnet_form.inc
 *
 * This file contains the form related functions(like admin reorder forms and payment form) to make
 * the payment process in the shop systems(E.g: Extension features provide by Novalnet).
 *
 */

    module_load_include('inc', 'uc_novalnet', 'includes/uc_novalnet_admin');

/**
 *
 * Implementation of global configuration for novalnet payments
 *
 * Global configuration form that contains the callback script and payment related configuration
 *
 * $form, &$form_state(referenced) - core variables
 *
 * @param  array $form
 * @param  array $form_state
 * @return array $form
 */
function uc_novalnet_global_config_form($form, &$form_state) {
    $base_url = url('', array('relative' => TRUE));
    drupal_add_css(drupal_get_path('module', 'uc_novalnet') . '/css/uc_novalnet.css');
    $module_url = drupal_get_path('module', 'uc_novalnet');
    $module_url = url($module_url, array('relative' => TRUE));

    /* To load the admin portal in shop back-end */
    $admin_portal  = t("For additional configurations login to %sNovalnet Merchant Administration Portal.%s To login to the Portal you need to have an account at Novalnet. If you don't have one yet, please contact %ssales@novalnet.de%s / tel. +49 (089) 923068320");
    $paypal_config = t('To use the PayPal payment method please enter your PayPal API details in %sNovalnet Merchant Administration portal%s');
    $form['merchant_details'] = array(
        '#markup' => sprintf($admin_portal, "<a href='https://admin.novalnet.de/' target='_blank'>", "</a>", "<a href='mailto:sales@novalnet.de' target='_blank'>", "</a>") . '<br/>'
    );
    $form['paypal_account'] = array(
        '#markup' => sprintf($paypal_config, "<a href='https://admin.novalnet.de/' target='_blank'>", "</a>") . '<br/>'
    );
    $form['novalnet_settings'] = array(
        '#type' => 'vertical_tabs',
    );
    /* To get Novalnet basic params from the shop back-end */
    $basic_data = basic_params();
    $cbk_data   = variable_get('novalnet_cbk_data', '');
    /* Global configuration field-set (Line no 64 - 164) - contains the payment releated configurations(E.g.: Vendor id, payment access key etc.)*/
    $form['global_config'] = array(
        '#type'        => 'fieldset',
        '#title'       => t('Global Configuration'),
        '#collapsible' => TRUE,
        '#collapsed'   => TRUE,
        '#group'       => 'novalnet_settings',
    );
    $form['global_config']['nn_global_loading'] = array(
        '#type'    => 'markup',
        '#markup'  => '<div id="nn_global_loading" style="display:none" data-val="0"><img src="' . $module_url . '/img/novalnet_loader.gif"></div>'
    );
    $form['global_config']['public_key'] = array(
        '#type'             => 'textfield',
        '#description'      => t('Enter Novalnet Product activation key'),
        '#title'            => t('Product activation key'),
        '#attributes'       => array('id' => 'publicKey', 'autocomplete' => 'off'),
        '#required'         => TRUE,
        '#default_value'    => isset($basic_data['public_key'])? $basic_data['public_key'] : '',
    );
    
    $form['base_url'] = array(
        '#type'           => 'hidden',
        '#attributes'     => array('id' => 'base_url'),
        '#value'          => $base_url
    );
    $form['global_config']['vendor_id'] = array(
        '#type'             => 'hidden',
        '#title'            => t('Merchant ID'),
        '#default_value'    => isset($basic_data['vendor_id'])? $basic_data['vendor_id'] : '',
        '#attributes'       => array('readonly' => 'readonly', 'id' => 'vendor_id'),
        '#required'         => TRUE,
        '#validated'        => TRUE,
        '#element_validate' => array('uc_novalnet_element_validate_number'),
    );
    $form['global_config']['auth_code'] = array(
        '#type'             => 'hidden',
        '#title'            => t('Authentication code'),
        '#attributes'       => array('readonly' => 'readonly', 'id' => 'auth_code'),
        '#required'         => TRUE,
        '#validated'        => TRUE,
        '#default_value'    => isset($basic_data['auth_code'])? $basic_data['auth_code'] : '',
    );
    $form['global_config']['product_id'] = array(
        '#type'             => 'hidden',
        '#title'            => t('Project ID'),
        '#attributes'       => array('readonly' => 'readonly', 'id' => 'product_id'),
        '#required'         => TRUE,
        '#validated'        => TRUE,
        '#default_value'    => isset($basic_data['product_id'])? $basic_data['product_id'] : '',
        '#element_validate' => array('uc_novalnet_element_validate_number'),
    );
    $form['global_config']['tariff_id'] = array(
        '#type'             => 'select',
        '#attributes'     => array('id' => 'tariff_id'),
        '#description'      => t('Select Novalnet tariff ID'),
        '#title'            => t('Tariff ID'),
        '#required'         => TRUE,
        '#validated'        => TRUE,
        '#options'          => !variable_get('novalnet_tariff_options') ? variable_get('novalnet_tariff_options', array(0 => t('--Select--'))) : variable_get('novalnet_tariff_options'),
        '#default_value'    => isset($basic_data['tariff_id'])? $basic_data['tariff_id'] : '',
    );
    $form['global_config']['payment_access_key'] = array(
        '#type'           => 'hidden',
        '#attributes'     => array('readonly' => 'readonly', 'id' => 'payment_access_key'),
        '#title'          => t('Payment access key'),
        '#required'       => TRUE,
        '#validated'      => TRUE,
        '#default_value'  => isset($basic_data['payment_access_key'])? $basic_data['payment_access_key'] : '',
    );

    $form['global_config']['paymentlogo'] = array(
        '#type'           => 'select',
        '#title'          => t('Display payment method logo'),
        '#description'    => t('The payment method logo will be displayed on the checkout page'),
        '#options'        => array( 1 => t('True'), 0 => t('False')),
        '#default_value'  => isset($basic_data['paymentlogo'])? $basic_data['paymentlogo'] : 1
    );
    $form['global_config']['onhold_transaction_status'] = array(
        '#type'          => 'select',
        '#title'         => t('Onhold order status'),
        '#options'       => uc_order_status_options_list(),
        '#default_value' => (isset($basic_data['onhold_transaction_status']))? $basic_data['onhold_transaction_status'] : 'pending',
    );
    $form['global_config']['onhold_void_cancel_status'] = array(
        '#type'          => 'select',
        '#title'         => t('Cancellation order status'),
        '#options'       => uc_order_status_options_list(),
        '#default_value' => (isset($basic_data['onhold_void_cancel_status']))? $basic_data['onhold_void_cancel_status'] : 'canceled',
    );
    $form['global_config']['referrer_id'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Referrer ID'),
        '#description'      => t('Enter the referrer ID of the person/company who recommended you Novalnet'),
        '#default_value'    => isset($basic_data['referrer_id'])? $basic_data['referrer_id'] : '',
        '#element_validate' => array('uc_novalnet_element_validate_number'),
        '#required'         => FALSE,
    );
    $form['global_config']['gateway_timeout'] = array(
		'#type'             => 'textfield',
		'#title'            => t('Gateway timeout (in seconds)'),
		'#description'      => t('In case the order processing time exceeds the gateway timeout, the order will not be placed'),
		'#default_value'    => isset($basic_data['gateway_timeout']) ? $basic_data['gateway_timeout'] : 240,
		'#element_validate' => array('uc_novalnet_element_validate_number'),
		'#required'         => FALSE,
	);
    /* Callback script configuration field-set (Line no 231 - 301) - contains the Callback releated configurations(E.g.: test mode etc.) */
    $form['callback_config'] = array(
        '#type'        => 'fieldset',
        '#title'       => t('Merchant script management'),
        '#collapsible' => TRUE,
        '#collapsed'   => TRUE,
        '#group'       => 'novalnet_settings',
    );
    $form['callback_config']['cbk_mail_test_mode'] = array(
        '#type'          => 'select',
        '#title'         => t('Deactivate IP address control (for test purpose only)'),
        '#options'       => array(1 => t('True'), 0 => t('False')),
        '#description'          => t('This option will allow performing a manual execution. Please disable this option before setting your shop to LIVE mode, to avoid unauthorized calls from external parties (excl. Novalnet).'),
        '#default_value'    => (!empty($cbk_data['cbk_mail_test_mode']))? $cbk_data['cbk_mail_test_mode'] : 0,
    );

    $form['callback_config']['cbk_mail_send'] = array(
        '#type'              => 'select',
        '#title'             => t('Enable E-mail notification for callback'),
        '#options'           => array(0 => t('True'), 1 => t('False')),
        '#default_value'     => (!empty($cbk_data)) ? $cbk_data['cbk_mail_send'] : 1,
        '#attributes'        => array('id' => 'nn_cbk_mail_send'),
    );
    $form['callback_config']['cbk_mail_to'] = array(
        '#type'              => 'textfield',
        '#title'             => t('E-mail address (To)'),
        '#description'       => t('E-mail address of the recipient'),
        '#default_value'     => (!empty($cbk_data))? $cbk_data['cbk_mail_to'] : '',
        '#element_validate'  => array('uc_novalnet_element_validate_email'),
        '#states'            => array('visible' => array('select[id="nn_cbk_mail_send"]' => array('value' => 0)))
    );
    $form['callback_config']['cbk_mail_bcc'] = array(
        '#type'           => 'textfield',
        '#required'       => FALSE,
        '#title'          => t("E-mail address (Bcc)"),
        '#description'    => t('E-mail address of the recipient for BCC'),
        '#default_value'  => (!empty($cbk_data['cbk_mail_bcc']))? $cbk_data['cbk_mail_bcc'] : '',
        '#states'         => array('visible' => array('select[id="nn_cbk_mail_send"]' => array('value' => 0)))
    );
    $form['callback_config']['cbk_mail_subject'] = array(
        '#type'              => 'textfield',
        '#title'             => t('E-mail subject'),
        '#default_value'     => !empty($cbk_data['cbk_mail_subject']) ? $cbk_data['cbk_mail_subject'] : t('Novalnet callback script notification'),
        '#states'            => array('visible' => array('select[id="nn_cbk_mail_send"]' => array('value' => 0)))
    );
    $form['callback_config']['cbk_mail_body'] = array(
        '#type'              => 'textarea',
        '#title'             => t('E-mail body'),
        '#default_value'     => (!empty($cbk_data))? $cbk_data['cbk_mail_body'] : '',
        '#states'            => array('visible' => array('select[id="nn_cbk_mail_send"]' => array('value' => 0)))
    );
    $form['callback_config']['notify_url'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Notification URL'),
        '#description'   => t('The notification URL is used to keep your database/system actual and synchronizes with the Novalnet transaction status'),
        '#value'         =>  url('callback_novalnet2ubercart', array('absolute' => TRUE)),
        '#default_value' => (isset($basic_data['notify_url']))? $basic_data['notify_url'] : '',
    );  
    
    /* Submit hanlder for Novalnet global configuration */
    $form['submit']  = array(
        '#type'             => 'submit',
        '#value'            => t('Save configuration'),
        '#submit'           => array('uc_novalnet_globalconfig_form_submit'),
    );
    return $form;
}

/**
 *
 * Submit handler for Global configuration form
 *
 * Function that used to save the global configuration details in drupal variables
 *
 * @param  array $form
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_globalconfig_form_submit($form, &$form_state) {
    $config =  array_map('trim' , $form_state['values']);
   
    $basic_params = variable_get('novalnet_params', array());
    if(!empty($basic_params['vendor_id'])) {
    /* Assigning the configured global confg values to Novalnet params */
    variable_set('novalnet_params', array_merge($basic_params, array(
        'public_key'                 => $config['public_key'],
		'onhold_transaction_status'  => $config['onhold_transaction_status'],
		'onhold_void_cancel_status'  => $config['onhold_void_cancel_status'],
		'referrer_id'                => $config['referrer_id'],
		'gateway_timeout'            => $config['gateway_timeout'],
        'is_details_saved'           => TRUE,
        'tariff_id'                  => $config['tariff_id'],
        'paymentlogo'                => $config['paymentlogo'],
        'referrer_id'                => $config['referrer_id'],
        'onhold_transaction_status'  => $config['onhold_transaction_status'],
        'onhold_void_cancel_status'  => $config['onhold_void_cancel_status'],
        'notify_url'                 => $config['notify_url'],
    )));
    variable_set('novalnet_cc_key', $config['payment_access_key']);
    /* Assigning the configured callback values to Novalnet params */
    variable_set('novalnet_cbk_data', array(
        'cbk_mail_send'          => $config['cbk_mail_send'],
        'cbk_mail_to'            => $config['cbk_mail_to'],
        'cbk_mail_subject'       => $config['cbk_mail_subject'],
        'cbk_mail_bcc'           => $config['cbk_mail_bcc'],
        'cbk_mail_test_mode'     => $config['cbk_mail_test_mode'],
        'cbk_mail_body'          => $config['cbk_mail_body'],
    ));
    drupal_set_message(t('Your Basic Configuration has been saved.') . ' ' . t('Please configure') . ' ' . t('Payment method') . 's ' . l(t('Here'), 'admin/store/settings/payment'));
}
}

/**
 *
 * Submit handler for Auto public API
 *
 * Function that used to send the Auto API request to payment server for refilling the
 *
 * vendor details in respective form fields.
 *
 * @param  array $form
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_publickey_submit() {
    global $language;
    /* Checking the public key is non-empty*/	
    if (!empty($_REQUEST['api_config_hash'])) {
        $data['hash'] = trim($_REQUEST['api_config_hash']);
        $data['lang']       = $language->language;
        $query = http_build_query($data, '', '&');
        $options = array(
            'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
            'method'  => 'POST',
            'data'    => $query
        );
        $basic_params = variable_get('novalnet_params', '');
        $host      = uc_novalnet_get_url('novalnet_auto_config');
        $response  = drupal_http_request($host, $options);
        $response  = json_decode($response->data);        
        if (!empty($response->vendor)) {			
            variable_set('novalnet_params', array(
                'public_key'                 => $data['hash'],
                'vendor_id'                  => $response->vendor,
                'auth_code'                  => $response->auth_code,
                'product_id'                 => $response->product,
                'payment_access_key'         => $response->access_key,
                'onhold_transaction_status'  => 'pending',
                'onhold_void_cancel_status'  => 'canceled',
                'referrer_id'                => $basic_params['referrer_id']
            ));
            $tariff = $response->tariff;
            foreach ($tariff as $key => $value) {
                if ($value->type == '2' ||$value->type == '3') {
                    $tariffVal[$key] = $value->name;
                }
            }
            /* Stores the tariff id's in drupal variable */
            variable_set('novalnet_tariff_options', $tariffVal);
            drupal_json_output(array('status' => 100, 'data' => $response));

        } else {
            variable_set('novalnet_params', '');
            /* Stores the tariff id's in drupal variable */
            variable_set('novalnet_tariff_options', array());
            drupal_json_output(array('status' => 1, 'data' => $response));
        }
	}
	else {
			variable_set('novalnet_params', '');
            /* Stores the tariff id's in drupal variable */
            variable_set('novalnet_tariff_options', array());
            drupal_json_output(array('status' => 0, 'data' => $response));
	}
    
}

/**
 *
 * Back end configuration for Individual payments
 *
 * Function that used to create the individual payment configuration form(E.g.: Fraud preventions, test mode etc.)
 *
 * @param  array $payment
 * @return array $form
 */
function uc_novalnet_backend_config($payment) {
    /* To get Novalnet basic params from the shop back-end */
    $config_data = basic_params();
    /* error message will triggered - when Global configuration is not configured, instead of configuring the individual payment configuration */
    if (empty($config_data)) {
        drupal_set_message(t('Please configure Novalnet Global Configuration'), 'error');
        drupal_goto('admin/novalnet/globalconfig');
    }
    $desc = uc_novalnet_payment_methods($payment);
    $form = array();
    $form['nn_title'] = array(
        '#title' => t('Novalnet Payment Settings'),
    );    
    $base_url = url('admin/novalnet/globalconfig', array('relative' => TRUE));
    $global  = t("For additional configurations go to %sNovalnet  Global Configuration.%s");
    $form['global_link'] = array(
            '#markup' => sprintf($global, "<a href='" . $base_url . "'>", "</a>") . '<br/>'
    );
    $form[$payment.'_payment_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Payment Title'),
        '#default_value' => variable_get($payment.'_payment_title', t($desc['name'])),
        '#description' => t('Help message shown at checkout.'),
    );
    $form[$payment.'_novalnet_description'] = array(
        '#type' => 'textarea',
        '#title' => t('Payment Description'),
        '#default_value' => variable_get($payment.'_novalnet_description', t($desc['desc'])),
        '#description' => t('Help message shown at checkout.'),
    );
    $form[$payment . '_test_mode'] = array(
        '#type'                 => 'select',
        '#title'                => t('Enable test mode'),
        '#description'          => t('The payment will be processed in the test mode therefore amount for this transaction will not be charged'),
        '#default_value'        => variable_get($payment . '_test_mode', 0),
        '#options'              => array( 1 => t('True'), 0 => t('False')),
    );
    /* One step check-out and zero amount booking flags for allowed payments (Line no 448 - 460) */
    if (in_array($payment, array('novalnet_cc', 'novalnet_sepa', 'novalnet_paypal'))) {
        $zero_amount_options = array( 0 => t('None'), 1 => t('One click shopping'), 2 => t('Zero amount booking'));

        $form[$payment . '_enable_one_shop_clicking'] = array(
            '#type'          => 'select',
            '#title'         => t('Shopping type'),
            '#description'   => t('Select shopping type'),
            '#options'       => $zero_amount_options,
            '#default_value' => variable_get($payment . '_enable_one_shop_clicking', 0),
        );
    }
    /* One step check-out and zero amount booking flags for allowed payments (Line no 448 - 460) */
    if (in_array($payment, array('novalnet_cc', 'novalnet_sepa', 'novalnet_paypal','novalnet_invoice'))) {
        $form[$payment.'_paymentoption'] = array(
            '#type'           => 'select',
            '#title'          => t('Payment action'),
            '#options'        => array( 'capture' => t('Capture'), 'authorize' => t('Authorize')),
            '#default_value'  => variable_get($payment.'_paymentoption') ? variable_get($payment.'_paymentoption') : 1,
             '#attributes'        => array('id' => $payment.'_paymentoption'),
        );
        $form[$payment.'_manualamount'] = array(
            '#type'          => 'textfield',
            '#title'         => t('Minimum transaction limit for authorization (in minimum unit of currency. E.g. enter 100 which is equal to 1.00)'),
            '#description'   => t('In case the order amount exceeds the mentioned limit, the transaction will be set on-hold till your confirmation of the transaction. You can leave the field empty if you wish to process all the transactions as on-hold.'),
            '#default_value' => variable_get($payment.'_manualamount') ? variable_get($payment.'_manualamount') : '',
            '#required'      => FALSE,
            '#element_validate' => array('uc_novalnet_element_validate_number'),
            '#states'        => array('visible' => array('select[id="'.$payment.'_paymentoption'.'"]' => array('value' => 'authorize')))
        );
    }
    if ($payment == 'novalnet_paypal') {
        $form[$payment . '_notification'] = array(
            '#type'          => 'fieldset',
            '#title'         => t('Note:'),
            '#attributes' => array('style' => "background-color:#ffff99; color:#0074BD"),
            '#description'   => t('In order to use this option you must have billing agreement option enabled in your PayPal account. Please contact your account manager at PayPal.'),
            '#states'        => array('invisible' => array('select[id="edit-novalnet-paypal-enable-one-shop-clicking"]' => array('value' => 0))),
      );
    }
    /* Specific configuration fields for CC payment(E.g.: CC expiry year limit etc.) Line 463 - 510 */
    if ($payment == 'novalnet_cc') {
        $form['novalnet_cc_cc3d'] = array(
            '#type'          => 'checkbox',
            '#title'         => '<b>' . t('Enable 3D secure') . '</b>',
            '#description'   => t('The 3D-Secure will be activated for credit cards. The issuing bank prompts the buyer for a password what, in turn, help to prevent a fraudulent payment. It can be used by the issuing bank as evidence that the buyer is indeed their card holder. This is intended to help decrease a risk of charge-back.'),
            '#default_value' => variable_get('novalnet_cc_cc3d', ''),
        );
        $form['novalnet_cc_force_cc3d'] = array(
            '#type'          => 'checkbox',
            '#title'         => '<b>' . t('Force 3D secure on predefined conditions') . '</b>',
            '#description'   => t('If 3D secure is not enabled in the above field, then force 3D secure process as per the “Enforced 3D secure (as per predefined filters & settings)” module configuration at the Novalnet Merchant Administration portal. If the predefined filters & settings from Enforced 3DSecure module are met, then the transaction will be processed as 3D secure transaction otherwise it will be processed as non 3D secure. Please note that the “Enforced 3D secure (as per predefined filters & settings)” module should be configured at Novalnet Merchant Administration portal prior to the activation here. For further information, please refer the description of this fraud module at “Fraud Modules” tab, below “Projects” menu, under the selected project in Novalnet Merchant Administration portal or contact novalnet support team.'),
            '#default_value' => variable_get('novalnet_cc_force_cc3d', ''),
        );
        $form['novalnet_cc_amex'] = array(
            '#type'          => 'select',
            '#title'         => t('Display AMEX logo'),
            '#description'   => t('Display AMEX logo in checkout page'),
            '#default_value' => variable_get('novalnet_cc_amex', 0),
            '#options'       => array( 1 => t('True'), 0 => t('False')),
        );
        $form['novalnet_cc_maestro'] = array(
            '#type'          => 'select',
            '#title'         => t('Display Maestro logo'),
            '#description'   => t('Display Maestro logo in checkout page'),
            '#default_value' => variable_get('novalnet_cc_maestro', 0),
            '#options'       => array( 1 => t('True'), 0 => t('False')),
        );
        $form['novalnet_cc_style'] = array(
            '#type'          => 'fieldset',
            '#title'         => t('CSS settings for Credit Card iframe'),
            '#collapsible' => TRUE,
            '#collapsed'   => TRUE,
        );
        $form['novalnet_cc_style']['common_label_style'] = array(
            '#type'          => 'textfield',
            '#title'         => t('Label'),
            '#description'   => t('E.g:') . 'color:#999999; background-color:#FFFFFF;',
            '#default_value' => variable_get('common_label_style') ? variable_get('common_label_style') : ''
        );
        $form['novalnet_cc_style']['common_field_style'] = array(
            '#type'          => 'textarea',
            '#title'         => t('Input'),
            '#description'   => t('E.g.') . 'color:#999999; background-color:#FFFFFF;',
            '#default_value' => variable_get('common_field_style') ? variable_get('common_field_style') : 'border-color: #cccccc; border-radius: 4px; box-shadow: 0 1px 1px rgba(0, 0, 0, 0.075) inset; color: #000; height: 34px; padding: 6px 12px; transition: border-color 0.15s ease-in-out 0s, box-shadow 0.15s ease-in-out 0s;'
        );
        $form['novalnet_cc_style']['common_style_text'] = array(
            '#type'          => 'textarea',
            '#title'         => t('CSS Text'),
            '#description'   => t('E.g:') . '#idselector{color:#999999;}.classSelector{color:#000000}',
            '#default_value' =>  variable_get('common_style_text') ? variable_get('common_style_text') : 'body{ color: #9a9a9a; font-family: sans-serif; font-size: 13px } .input-group{ width: 50%; float:left; margin-bottom: 2px } .label-group{ margin-bottom: 5px; } .form-group{ padding-right: 15px; } .form-group label{ text-transform: uppercase; font-size: 12}'
        );
        $form['novalnet_cc_key'] = array(
            '#type'           => 'hidden',
            '#default_value'  => variable_get('novalnet_cc_key', '')
        );
    }
        /* Order status configuration for individula payments */
    $cat_option = uc_order_status_options_list();
    if (in_array($payment, array('novalnet_prepayment', 'novalnet_invoice', 'novalnet_paypal', 'novalnet_przelewy24', 'novalnet_cashpayment'))) {
        $form[$payment . '_before_status'] = array(
            '#type'          => 'select',
            '#title'         => in_array($payment, array('novalnet_paypal', 'novalnet_przelewy24')) ? t('Order status for the pending payment') : t('Order completion status'),
            '#options'       => $cat_option,
            '#default_value' => variable_get($payment . '_before_status', 'pending'),
        );
    }
    $form[$payment . '_after_status'] = array(
        '#type'          => 'select',
        '#title'         => in_array($payment, array('novalnet_prepayment', 'novalnet_invoice', 'novalnet_cashpayment')) ? t('Callback order status') : t('Order completion status'),
        '#options'       => $cat_option,
        '#default_value' => variable_get($payment . '_after_status', 'completed'),
    );
    if (in_array($payment, array('novalnet_sepa', 'novalnet_invoice'))) {
        /* Allowed payment methods for fraud prevention */
        $form['fraud_prevention'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Risk and control management'),
            '#collapsible'      => TRUE,
            '#collapsed'        => TRUE
            );
        $form['fraud_prevention'][$payment . '_pin_method'] = array(
            '#type'                 => 'radios',
            '#title'                => t('Enable fraud prevention'),
            '#description'          => t('To authenticate the buyer for a transaction, the PIN will be automatically generated and sent to the buyer. This service is only available for customers from DE, AT, CH'),
            '#default_value'        => variable_get($payment . '_pin_method', '0'),
            '#options'              => array(0 => t('None'), 'callback' => t('PIN by callback'), 'sms' => t('PIN by SMS'))
        );
        $form['fraud_prevention'][$payment . '_pin_amount'] = array(
            '#type'           => 'textfield',
            '#title'          => t('Minimum value of goods') . ' ' . t('(in minimum unit of currency. E.g. enter 100 which is equal to 1.00)'),
            '#description'    => t('Enter the minimum value of goods from which the fraud module should be activated'),
            '#default_value'  => variable_get($payment . '_pin_amount', ''),
            '#element_validate' => array('uc_novalnet_element_validate_number'),
            '#states'           => array('invisible' => array('input[name="' . $payment . '_pin_method"]' => array('value' => 0))),
        );
        /* Allowed payment methods for Due date */
        $form[$payment . '_due_date'] = array(
            '#type'             => 'textfield',
            '#title'            => ($payment == 'novalnet_invoice') ? t('Payment due date (in days)') : t('SEPA payment duration (in days)'),
            '#description'      => ($payment == 'novalnet_invoice') ? t('Enter the number of days to transfer the payment amount to Novalnet (must be greater than 7 days). In case if the field is empty, 14 days will be set as due date by default') : t('Enter the number of days after which the payment should be processed (must be between 2 and 14 days)'),
            '#default_value'    => variable_get($payment . '_due_date', ''),
            '#element_validate' => array('uc_novalnet_element_validate_duedate'),
        );
        /* Allowed payments for Guaranteed payments */
        $form['payment_guarantee_validate_check'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Payment guarantee configuration'),
            '#collapsible'      => TRUE,
            '#collapsed'        => TRUE,
            '#element_validate' => array('payment_gurantee_validation'),
            );
        $form['payment_guarantee_validate_check'][$payment . '_payment_guarantee_check'] = array(
            '#type'                 => 'radios',
            '#title'                => t("Enable payment guarantee"),
            '#default_value'        => variable_get($payment . '_payment_guarantee_check', 0),
            '#options'              => array(1 => t('True'), 0 => t('False'))
            );
        $form['payment_guarantee_validate_check'][$payment . '_guarantee_before_status'] = array(
            '#type'          => 'select',
            '#title'         =>  t('Order status for the pending payment'),
            '#options'       => $cat_option,
            '#default_value' => variable_get($payment . '_guarantee_before_status', 'pending'),
        );
        $form['payment_guarantee_validate_check'][$payment . '_payment_guarantee_condition_check'] = array(
            '#type'             => 'markup',
            '#markup'           => '<label>' . t('Basic requirements for payment guarantee') . '</label>
                            <ul>
                            <li>' . t('Allowed countries: AT, DE, CH') . '</li>
                            <li>' . t('Allowed currency: EUR') . '</li>
                            <li>' . t('Minimum amount of order >= 9,99 EUR') . '</li>
                            <li>' . t('Minimum age of end customer >= 18 Years') . '</li>
                            <li>' . t('The billing address must be the same as the shipping address') . '</li>
                            <li>' . t('Gift certificates/vouchers are not allowed') . '</li>
                            </ul>',
        );
        $form['payment_guarantee_validate_check'][$payment . '_minimum_payment_guarantee_check'] = array(
            '#type'                 => 'textfield',
            '#title'                => t('Minimum order amount') . ' ' . t('(in minimum unit of currency. E.g. enter 100 which is equal to 1.00)'),
            '#description'          => t("This setting will override the default setting made in the minimum order amount. Note that amount should be in the range of 9,99 EUR."),
            '#default_value'        => variable_get($payment . '_minimum_payment_guarantee_check'),
        );
        $form['payment_guarantee_validate_check'][$payment . '_payment_guarantee_active'] = array(
            '#type'                 => 'radios',
            '#title'                => t("Force Non-Guarantee payment"),
            '#description'          => t("If the payment guarantee is activated (True), but the above mentioned requirements are not met, the payment should be processed as non-guarantee payment."),
            '#default_value'        => variable_get($payment . '_payment_guarantee_active', 1),
            '#options'              => array(1 => t('True'), 0 => t('False'))
        );
    }
    if ($payment == 'novalnet_cashpayment') {
        $form[$payment . '_due_date'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Slip expiry date (in days)'),
            '#description'      => t('Enter the number of days to pay the amount at store near you. If the field is empty, 14 days will be set as default.'),
            '#default_value'    => variable_get($payment . '_due_date', ''),
        );
    }
    /* Information to the end-customer */
    $form[$payment . '_user_info'] = array(
        '#type'              => 'textfield',
        '#title'             => t('Notification for the buyer'),
        '#description'       => t('The entered text will be displayed on the checkout page.'),
        '#default_value'     => variable_get($payment . '_user_info', ''),
        '#element_validate'  => array('uc_novalnet_sanitize_referid')
    );
    return $form;
}

/**
 *
 * Payment method callback function: display the payment form
 *
 * @param  object $order
 * @param  array $form_state
 * @return array $form
 */
function uc_novalnet_getform($form, $form_state, $order) {
    $data = array();
    $payment_method = $order->payment_method;
    $config_data = basic_params();
    $implementation = '';
    /* To get country details from the billing address */
    $billing_country = uc_get_country_data(array('country_id' => $order->billing_country));
    /* Guaranteed payments - Birthdate form */
    $show_calendar = FALSE;
    /* Guaranteed payments for allowed payments and for allowed countries */
    if (in_array($payment_method, array('novalnet_invoice', 'novalnet_sepa')) && !isset($_SESSION['novalnet_session'][$payment_method]['tid'])) {
      /* Check the guarantee payment active flag */
        if (variable_get($payment_method . '_payment_guarantee_check', 0)) {
            $delivery_address = $form_state['build_info']['args'];
            foreach ($delivery_address as $value) {
            $address_fields = $value;
            }
            $delivery_address = array($address_fields->delivery_street1, $address_fields->delivery_street2, $address_fields->delivery_city, $address_fields->delivery_postal_code, $address_fields->delivery_country);
            $billing_address = array($address_fields->billing_street1, $address_fields->billing_street2, $address_fields->billing_city, $address_fields->billing_postal_code, $address_fields->billing_country);
            $min_amount = variable_get($order->payment_method . '_minimum_payment_guarantee_check') ? variable_get($order->payment_method . '_minimum_payment_guarantee_check') : 999;
            $order_amt = $order->order_total * 100;
            
            /* Check the amount is greater than 9,99 euro */
            if (($order_amt >= $min_amount) && $order->currency == 'EUR' && in_array($billing_country[0]['country_iso_code_2'], array('AT', 'DE', 'CH')) && ($delivery_address === $billing_address)) {
                $show_calendar = TRUE;
                $implementation = 'gurantee';
                if (isset($_SESSION['novalnet_session'][$payment_method][$payment_method . '_pinkey']))
                unset( $_SESSION['novalnet_session'][$payment_method][$payment_method . '_pinkey'] );
            } 
        }
    }
    $zero_amount = uc_novalnet_zero_amount_booking($data, $payment_method, $config_data);
    /* Fraud prevention - checking the order amount with stored order amount */
    if ((isset($_SESSION['novalnet_session'][$payment_method]['amount']) && $_SESSION['novalnet_session'][$payment_method]['amount'] != (string) $order->order_total ) && (!$zero_amount)) {
        unset($_SESSION['novalnet_session']);
        drupal_set_message(t('The order amount has been changed, please proceed with the new order'), 'error');
    }
    /* Paypal perform one click process  */
    if ($payment_method == 'novalnet_paypal' && variable_get('novalnet_paypal_enable_one_shop_clicking', 0) == 1) {
        $form = uc_novalnet_get_paypal_form($order);
    }
    $form['payment_method'] = array(
        '#type'              => 'hidden',
        '#title'             => t('Payment method'),
        '#default_value'     => $payment_method,
        '#attributes' => array('id' => 'payment_method')
    );
    /* Allowed payments for Fraud prevention */
    if ($payment_method == 'novalnet_sepa' && !isset($_SESSION['novalnet_session'][$payment_method]['tid'])) {
        $form['nn_authcode'] = array(
            '#type'           => 'hidden',
            '#default_value'  => $config_data['auth_code'],
            '#attributes' => array('id' => 'nn_authcode'),
            '#prefix'     => '<noscript><div class="messages error">' . t('Please enable the Javascript in your browser to proceed further with the payment') . '</div><input type="hidden" name="noscript"><div style="display:none"></noscript>'
        );
        $form['nn_vendor'] = array(
            '#type'           => 'hidden',
            '#default_value'  => $config_data['vendor_id'],
            '#attributes' => array('id' => 'nn_vendor'),
        );
        $form['nn_form_loading'] = array(
            '#type'    => 'markup',
            '#markup'  => '<div id="loading-img"><img src="' . url(drupal_get_path('module', 'uc_novalnet'), array('relative' => TRUE)) . '/img/novalnet_loader.gif"></div>'
        );
        $form_fields  = uc_novalnet_get_sepa_form($order) ;
        $form = array_merge($form, $form_fields);
    }

    /* PCI form with one click shop enabled - show the masked details in checkout form */
    if ($payment_method == 'novalnet_cc')
    $form = uc_novalnet_get_cc_form($order);
     /* If Birth date is required */
    if ($show_calendar) {   
		
        $form['birth_date'] = array(
        '#type' => 'date', 
        '#title' => t('Your date of birth'), 
        '#element_validate' => array('uc_novalnet_element_validate_birth_date')
        );        
    }
    /* Checking Allowed countries, minimum value for Fraud prevention */
    if (((in_array($order->payment_method, array('novalnet_sepa', 'novalnet_invoice')) && in_array($billing_country[0]['country_iso_code_2'], array('AT', 'DE', 'CH')) && variable_get($order->payment_method . '_pin_method') && ( !variable_get($order->payment_method . '_pin_amount') || $order->order_total * 100 >= variable_get($order->payment_method . '_pin_amount'))) || isset($_SESSION['novalnet_session'][$order->payment_method]['tid'])) && $order->order_status == 'in_checkout' && empty($implementation) ) {
        /* Fraud prevention - If TID in session */
        if (!isset($_SESSION['novalnet_session'][$payment_method]['tid'])) {
            if (isset($_SESSION['novalnet_session'][$payment_method][$payment_method . '_birth_date']))
                unset( $_SESSION['novalnet_session'][$payment_method][$payment_method . '_birth_date'] );
            $form[$payment_method . '_pinkey'] = array(
                '#type'             => 'textfield',
                '#title'            => ((variable_get($payment_method . '_pin_method') == 'sms') ? t('Mobile number') : t('Telephone number')) . ' <span class="form-required" title="This field is required.">*</span>',
                '#default_value'    => isset($order->billing_phone) ? $order->billing_phone : '',
                '#attributes'       => array('autocomplete' => 'off', 'id' => 'fraud_number'),
                '#size'             => 25,
                '#element_validate' => array('uc_novalnet_element_validate_fraud_pin'),
            );
        } else {
            $form[$payment_method . '_pin'] = array(
                '#type'             => 'textfield',
                '#title'            => t('Transaction PIN') . ' <span class="form-required" title="This field is required.">*</span>',
                '#size'             => 20,
                '#element_validate' => array('uc_novalnet_element_validate_pin'),
                '#attributes'       => array('autocomplete' => 'off')
            );
            $form[$payment_method . '_forget_pin'] = array(
                '#type'             => 'checkbox',
                '#title'            => t('Forgot your PIN?'),
            );
        }
    }
    /* Displaying the test mode and payment description */
    $test_mode_info = (variable_get($payment_method . '_test_mode', 0)) ? '<div style =color:red;>' . t('The payment will be processed in the test mode therefore amount for this transaction will not be charged') . '</div>' : '';
    $user_info = ((bool) variable_get($payment_method . '_user_info', '')) ? variable_get($payment_method . '_user_info', '') . '<br />' : '';
    $zero_amount_desc = (variable_get($payment_method . '_enable_one_shop_clicking') == 2) ? t('This order will be processed as zero amount booking which store your payment data for further online purchases.') . '<br />'  : '';
    
    $form[$payment_method . 'payment_info'] = array(
                        '#markup' => '<div style="clear:both">' . $test_mode_info . $user_info . $zero_amount_desc .'</div>',
                        );
    return $form;
}

/**
 *
 * Function to validate gurantee payment
 *
 * @param  object $order
 * @param  array $country
 * @param  array $delivery_address
 * @param  array $billing_address
 * @param  null $process
 * @return array $form
 */
function uc_novalnet_gurantee_limit_validation($order, $country, $delivery_address, $billing_address, $birth_date, $company, $process = NULL) {
	$min_amount = variable_get($order->payment_method . '_minimum_payment_guarantee_check') ? variable_get($order->payment_method . '_minimum_payment_guarantee_check') : 999;
	$order_amt = $order->order_total * 100;
	$dob = (time() - strtotime($birth_date))/31536000;
    if (!in_array($country, array('AT', 'DE', 'CH')) || (($order_amt < $min_amount) || variable_get('uc_currency_code', 'USD') != 'EUR') || ($delivery_address != $billing_address) || (empty($company) && ($dob < 18))) {
		$gurantee_limits_error_msg = t('The payment cannot be processed, because the basic requirements for the payment guarantee haven\'t been met').'<br />';
          if ($delivery_address != $billing_address) {
            $gurantee_limits_error_msg .= '<li>'.t('The shipping address must be the same as the billing address').'<br /></li>';            
          } if ($order_amt < $min_amount) {
            $gurantee_limits_error_msg .= '<li>'.t('Minimum order amount must be @min_amount EUR', array('@min_amount' => $min_amount)).'<br /></li>';
          } if ((!in_array($country, array('AT', 'DE', 'CH')))) {
            $gurantee_limits_error_msg .= '<li>'.t('Only Germany, Austria or Switzerland are allowed').'<br /></li>';
          } if (variable_get('uc_currency_code', 'USD') != 'EUR') {
            $gurantee_limits_error_msg .= '<li>'.t('Only EUR currency allowed').'<br /></li>';
          } if (empty($company) && ($dob < 18)) {
			$gurantee_limits_error_msg .= '<li>'.t('You need to be at least 18 years old').'</li>';
		  }
        drupal_set_message($gurantee_limits_error_msg, 'error');
                drupal_goto('cart/checkout');
    }
}

/**
 *
 * Payment method callback function: additional params formation for redirect payments
 *
 * Callback function for redirection payments - build the form fields and submit through post method
 *
 * @param  object $order
 * @param  array $form
 * @param  array $form_state
 * @return array $form
 */
function uc_novalnet_redirect_form($form, &$form_state, $order) {
    $data = array();
    $payment_method = $order->payment_method;
    $config_data = basic_params();
    $form_action = uc_novalnet_get_url($payment_method);
    uc_novalnet_getparams( $data, $order);
        $return_url                   = url('novalnet_order/review',  array('absolute' => TRUE));
        $data['return_url']           = $return_url;
        $data['return_method']        = 'POST';
        $data['error_return_url']     = $return_url;
        $data['error_return_method']  = 'POST';
        unset($data['payment_ref']);
    if ($payment_method == 'novalnet_cc' && (variable_get('novalnet_cc_cc3d') == 1 || variable_get('novalnet_cc_force_cc3d') == 1 )) {
        if (variable_get($payment_method . '_enable_one_shop_clicking') == 1)
            unset($data['create_payment_ref'], $data['payment_ref']);
        if (variable_get($payment_method . '_enable_one_shop_clicking') == 2)
            $_SESSION['novalnet_session'][$payment_method]['request'] = serialize($data);
            $form_action = uc_novalnet_get_url('novalnet_cc_pci');
    }
    foreach ($data as $name => $value) {
      $form[$name] = array('#type' => 'hidden', '#value' => $value);
    }
    uc_novalnet_save_log($order->order_id, $data, 'request');
    $form['#action'] = $form_action;
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit order'),
    );
    return $form;
}

/**
 *
 * Payment method callback function: to store transaction request and responses
 *
 * @param  int $order_id
 * @return array $form
 */
function uc_novalnet_payment_logs($order_id) {
    $callback = array();
   /* Get the logs from the Novalnet transaction logs */
    $order    = db_query("SELECT logtype, logvalue, logtime FROM  {uc_novalnet_logs} WHERE order_id = :id", array(':id' => $order_id->order_id)) -> fetchAll();

    foreach ($order as $val) {
        /* Divide the logs as per the logtype */
        switch ($val->logtype) {
            case 'request':
                $request[] = $val;
            break;
            case 'response':
                $response[] = $val;
            break;
            case 'callback':
                $callback[] = $val;
            break;
        }
    }
    /* Novalnet log field set */
    $form['novalnet_logs'] = array(
        '#type' => 'fieldset',
        '#title' => t('Novalnet Transactions Log') . '  -  ' . $order_id->order_id,
        '#collapsible'  => TRUE,
        '#collapsed'    => FALSE
    );
    /* Fieldset that contains the payment request made to payment server */
    $form['novalnet_logs']['request'] = array(
        '#type' => 'fieldset',
        '#title' => t('Novalnet Request Data'),
        '#collapsible'  => TRUE,
        '#collapsed'     => TRUE
    );
    /* Fieldset that contains the payment response recieved from the payment server */
    $form['novalnet_logs']['response'] = array(
        '#type' => 'fieldset',
        '#title' => t('Novalnet Response Data'),
        '#collapsible'  => TRUE,
        '#collapsed'     => TRUE
    );
    /* Fieldset that contains callback request made to payment server */
    $form['novalnet_logs']['callback'] = array(
        '#type' => 'fieldset',
        '#title' => t('Novalnet Merchant script'),
        '#collapsible'  => TRUE,
        '#collapsed'     => TRUE
    );
    $message  = t('No Results Found');
    /* Displays the payment request */
    if (!empty($request)) {
        foreach ($request as $rqval) {
            $form['novalnet_logs']['request']['request_data'][] = array(
                '#type' => 'textarea',
                '#title' => date('d.m.Y H:i:s ', strtotime(date($rqval->logtime))),
                '#value' => print_r(unserialize($rqval->logvalue), TRUE),
                '#attributes' => array('readonly' => 'readonly')
            );
        }
    } else $form['novalnet_logs']['request']['request_data']['#markup'] = $message;
    /* Displays the payment response */
    if (!empty($response)) {
        foreach ($response as $rsval) {
            $form['novalnet_logs']['response']['response_data'][] = array(
                '#type' => 'textarea',
                '#title' => date('d.m.Y H:i:s ', strtotime(date($rsval->logtime))),
                '#value' => print_r(unserialize($rsval->logvalue), TRUE),
                '#attributes' => array('readonly' => 'readonly')
            );
        }
    } else $form['novalnet_logs']['response']['request_data']['#markup'] = $message;
    /* Displays the callback request */
    if (!empty($callback)) {
        foreach ($callback as $urls) {
            $form['novalnet_logs']['callback']['response_data'][] = array(
                '#type' => 'textarea',
                '#title' => date('d.m.Y H:i:s ', strtotime(date($urls->logtime))),
                '#value' => print_r(unserialize($urls->logvalue), TRUE),
                '#attributes' => array('readonly' => 'readonly')
            );
        }
    }
    else $form['novalnet_logs']['callback']['request_data']['#markup'] =  $message;
    return $form;
}

/**
 *
 * Builds the Process payment button on the order view.
 *
 * Payment handler for admin reorder
 *
 * @param  array $form
 * @param  array $form_state
 * @param  string $order_id
 * @return array $form
 */
function uc_novalnet_order_view_form($form, &$form_state, $order_id) {
    $form['order_id'] = array(
        '#type' => 'hidden',
        '#value' => $order_id,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Process payment'),
    );
    return $form;
}

/**
 *
 * Submit handler for order view form.
 *
 * Admin reorder menu callback
 *
 * @param  array $form
 * @param  array $form_state
 * @return null
 */
function uc_novalnet_order_view_form_submit($form, &$form_state) {
    $form_state['redirect'] = 'admin/store/orders/' . $form_state['values']['order_id'] . '/novalnet_payment';
}

/**
 *
 * Displays the payment form for administrators.
 *
 * @param  array $form
 * @param  array $form_state
 * @param  object $order
 * @return array $form
 */
function uc_novalnet_admin_reorder_form($form, &$form_state, $order) {
    /* Fetch the order balance from the order object */
    uc_payment_balance($order);
    /* Fetch the payment method name from order object */
    $payment_method = $order->payment_method;
    $config_data = basic_params();
    /* Test mode description */
    $test_mode_info = (variable_get($payment_method . '_test_mode', 0)) ? '<br /><div style =color:red;>' . t('The payment will be processed in the test mode therefore amount for this transaction will not be charged') . '</div>' : '';
    $payment_description = uc_novalnet_payment_methods($payment_method);

    $user_info = ((bool) variable_get($payment_method . '_user_info', '')) ? variable_get($payment_method . '_user_info', '') . '<br />' : '';

    $form['payment_method'] = array(
        '#type'              => 'hidden',
        '#title'             => t('Payment method'),
        '#default_value'     => $order->payment_method,
        '#attributes' => array('id' => 'nn_admin_payment')
    );
    $form['nn_order_on'] = array(
        '#type'              => 'hidden',
        '#default_value'     => $order->order_id,
    );
    $form_type = '';
    /* Based on the payment method - reorder form will getting loaded from the respective payment handler */
    switch ($payment_method) {
        case 'novalnet_cc':
            /* Check the CC PCI flag is activated - If TRUE, it will show the CC PCI form */
            if (variable_get('novalnet_cc_cc3d') == 1 || variable_get('novalnet_cc_force_cc3d') == 1) {
                $form['nn_cc_admin'] = array(
                    '#type'             => 'hidden',
                    '#attributes'       => array("id" => "nn_cc_admin"),
                    '#default_value'    => 1,
                );
                $form['pan_hash'] = array(
                    '#type'             => 'hidden',
                    '#attributes'       => array("id" => "nn_cc_hash"),
                    '#default_value'    => '',
                );
                $form['novalnet_cc_uniqueid'] = array(
                    '#type'             => 'hidden',
                    '#attributes'       => array("id" => "novalnet_cc_uniqueid"),
                    '#default_value'    => '',
                );
                $form['cc'] = uc_novalnet_getform($form, $form_state, $order);
                $form = array_merge(drupal_get_form('uc_novalnet_redirect_form', $order), $form);
                $form_type = 'redirect';
            } else {
                $form = array_merge(drupal_get_form('uc_novalnet_getform', $order), $form);
            }
        break;
        case 'novalnet_invoice':
		case 'novalnet_prepayment':
		case 'novalnet_cashpayment':
		case 'novalnet_sepa':
			$form = array_merge(drupal_get_form('uc_novalnet_getform', $order), $form);
		break;
		case 'novalnet_paypal':
		case 'novalnet_ideal':
		case 'novalnet_eps':
		case 'novalnet_instantbank':
		case 'novalnet_giropay':
		case 'novalnet_przelewy24':

		/* fetch the payment description for redirect payments */
		$form['redirect'] = uc_novalnet_getform($form, $form_state, $order);
		$form = array_merge(drupal_get_form('uc_novalnet_redirect_form', $order), $form);
		$form_type = 'redirect';
		break;
	}
    if ($form_type == 'redirect') {
		$form['description'] = array('#type' => 'markup', '#markup' => $payment_description['desc'] . '<br/>');
    } else {
            $form['description'] = array('#type' => 'markup', '#markup' => $payment_description['desc'] . '<br/>');
        $form['submit']   = array(
            '#type'       => 'submit',
            '#value'      => t('Submit order'),
            '#attributes' => array('id' => 'edit-continue'),
            '#submit'     => array('uc_novalnet_admin_reorder_form_submit')
        );
    }
    return $form;
}

/**
 *
 * Submit handler for uc_novalnet_admin_reorder_form
 *
 * @param  array $form
 * @param  array $form_state
 * @return null
 */
function uc_novalnet_admin_reorder_form_submit($form, &$form_state) {
    $order = uc_order_load($form_state['values']['nn_order_on']);
    uc_payment_method_novalnet_methods('order-submit', $order);
}

/**
 *
 * Payment method callback function: display the payment form of Direct Debit SEPA
 *
 * @param  object $order
 * @return array $form
 */
function uc_novalnet_get_paypal_form($order) {
    /* Checking One step checkout */
    $enable_paypal_one_shop = (variable_get('novalnet_paypal_enable_one_shop_clicking', 0) == 1) ? 1 : 0;

    /* To display the account details in masked format */
    $paypal_details    = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('masked_acc_details'), array('is_ref_order' => '1'), array('paymentkey' => 34));
    $form = array();
    if (!empty($paypal_details))
    $paypal_details = unserialize($paypal_details['masked_acc_details']);
    if ($enable_paypal_one_shop == 1 && !empty($paypal_details) && arg(0) != 'admin') {
        $form['one_shop_paypal_enabled'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array("id" => "one_shop_paypal_enabled"),
            '#default_value'    => $enable_paypal_one_shop
        );
        /* To display the flag to change the card details */
        $form['enable_paypal_one_shop'] = array(
            '#type'       => 'markup',
            '#attributes' => array('class' => 'enable_paypal_one_shop'),
            '#markup'     => '<span class="paypal_enter_new">' . t('Proceed with new PayPal account details') . '</span>',
        );
        $form['form_error_type_paypal'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array("id" => "form_error_type_paypal"),
            '#default_value'    => !empty($_SESSION['novalnet_session']['novalnet_paypal']['form_type']) ? 1 : ''
        );
        $form['paypal_given_details'] = array(
        '#type'                 => 'hidden',
        '#attributes'           => array('class' => 'paypal_given_details'),
        '#default_value'        => !empty($_SESSION['novalnet_session']['novalnet_paypal']['paypal_given_details']) ? $_SESSION['novalnet_session']['novalnet_paypal']['paypal_given_details'] : 1,
        );
        /* To display the paypal transaction id and Novalnet transaction id */
        $form['given_paypal_details'] = array(
            '#type'   => 'markup',
            '#markup' => "<div class='given_paypal_details' style=>
                      <table style='width: 50%;'><tr><td>" . t('PayPal transaction ID') . "</td><td>" . $paypal_details['paypal_transaction_id'] . " </td></tr>
                      <tr><td>" . t('Novalnet transaction ID%s', array('%s' => '')) . "</td><td>" . $paypal_details['tid'] . " </td></tr></table>",
        );
        $form['paypal_payment_ref'] = array(
            '#type'   => 'markup',
            '#markup' =>  t('Once the order is submitted, the payment will be processed as a reference transaction at Novalnet') . '</div>',
            '#element_validate' => array('uc_novalnet_nnpaypalform_validate'),
        );
    }
    return $form;
}

/**
 *
 * Payment method callback function: display the payment form of Direct Debit SEPA
 *
 * @param  object $order
 * @return array $form
 */
function uc_novalnet_get_sepa_form($order) {
      $billing_country          = uc_get_country_data(array('country_id' => $order->billing_country));
      $country                  = $billing_country[0]['country_iso_code_2'];
      /* Checking One step checkout */
      $enable_sepa_one_shop = (variable_get('novalnet_sepa_enable_one_shop_clicking', 0) == 1) ? 1 : 0;

      /* Fetch the masked account details from the Novalnet table */
      if ($order->uid != 0) {
          $get_guaranteed_key = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('paymentkey'), array('is_ref_order' => '1'));
          /* Fetch the payment key from transaction table */
          if (!in_array($get_guaranteed_key, array(37, 40)))
          $get_guaranteed_key = 37;
          /* Fetch the payment key from transaction table */
          $account_details = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('masked_acc_details'), array('is_ref_order' => '1'), array('paymentkey' => $get_guaranteed_key));
          $account_details = unserialize($account_details['masked_acc_details']);
      }

      if ($enable_sepa_one_shop == 1 && !empty($account_details) && arg(0) != 'admin') {
            $form['one_shop_sepa_enabled'] = array(
                '#type'             => 'hidden',
                '#attributes'       => array("id" => "one_shop_sepa_enabled"),
                '#default_value'    => $enable_sepa_one_shop
            );
            /* To display the flag to change the card details */
            $form['enable_sepa_one_shop'] = array(
                '#type'       => 'markup',
                '#attributes' => array('class' => 'enable_sepa_one_shop'),
                '#markup'     => '<span class="sepa_enter_new">' . t('Enter new account details') . '</span>',
            );
            /* To display the account details in masked format */
            $value = "<div class='given_sepa_details' style=> <table style='width: 50%;'>";
            $value .= "<tr><td>" . t('Account holder') . "</td><td>" . $account_details['bankaccount_holder'] . " </td></tr>";
            $value .= "<tr><td>" . t('IBAN or Account number') . "</td><td>" . $account_details['iban'] . " </td></tr>";
            $value .= "</table> </div>";
            /* To display the account details in masked format */
            $form['given_sepa_details'] = array(
                '#type'   => 'markup',
                '#markup' => $value,
            );
            $form['form_error_type_sepa'] = array(
                '#type'             => 'hidden',
                '#attributes'       => array("id" => "form_error_type_sepa"),
                '#default_value'    => !empty($_SESSION['novalnet_session']['novalnet_sepa']['form_type']) ? 1 : ''
            );
            $form['sepa_given_details'] = array(
                '#type'                 => 'hidden',
                '#attributes'           => array('class' => 'sepa_given_details'),
                '#default_value'        => !empty($_SESSION['novalnet_session']['novalnet_sepa']['sepa_given_details']) ? $_SESSION['novalnet_session']['novalnet_sepa']['sepa_given_details'] : 0
            );
      }

      /* Get the name of the user account */
      $form['common_sepa_hide'] = array(
          '#type'           => 'hidden',
          '#default_value'  => ($enable_sepa_one_shop == 1 && !empty($account_details)) ? 1 : 0,
          '#attributes'     => array('id' => 'common_sepa_hide'),
      );
      $sepa_owner               = trim($order->billing_first_name . ' ' . $order->billing_last_name);
      $form['sepa_one_div_first'] = array('#type' => 'markup', '#markup' => '<div class="sepa_one_div" >');
      $form['nnsepa_owner'] = array(
          '#type'          => 'textfield',
          '#title'         => t('Account holder'),
          '#default_value' => $sepa_owner,
          '#attributes'    => array('autocomplete' => 'off', 'id' => 'nnsepa_owner'),
          '#size'          => 25,
          '#maxlength'     => 64,
      );
      $form['nnsepa_iban'] = array(
          '#type'          => 'textfield',
          '#title'         => t('IBAN or Account number'),
          '#default_value' => '',
          '#attributes'    => array('autocomplete' => 'off', 'id' => 'nnsepa_iban' , 'style' => 'text-transform: uppercase;'),
          '#size'          => 25,
          '#maxlength'     => 64,
          '#element_validate' => array('uc_novalnet_nnsepaform_validate'),
      );
      $form['sepa_one_div_last'] = array('#type' => 'markup', '#markup' => '</div>');
      return $form;
}

/**
 *
 * Payment method callback function: display the payment form of Credit Card
 * @param  object $order
 * @return array $form
 */
function uc_novalnet_get_cc_form($order) {
    global $language;
    $card_details = array();
    /* To get Novalnet basic params */
    $config_data     = basic_params();
    $enable_one_shop = variable_get('novalnet_cc_enable_one_shop_clicking') == 1 ? TRUE : FALSE;
    if ($order->uid != 0) {
        $card_details    = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('masked_acc_details'), array('is_ref_order' => '1'), array('paymentkey' => 6));
        if (!empty($card_details))
        $card_details = unserialize($card_details['masked_acc_details']);
    }
    $one_shop_cc_enabled = !empty($enable_one_shop) && !empty($card_details) ? 1 : 0;
    $serverip = ( filter_var($_SERVER['SERVER_ADDR'], FILTER_VALIDATE_IP, FILTER_FLAG_IPV6) || $_SERVER['SERVER_ADDR'] == '::1' ) ? '127.0.0.1' : $_SERVER['SERVER_ADDR'];
    $fid = base64_encode('vendor='.$config_data['vendor_id'].'&product='.$config_data['product_id'].'&server_ip='.$serverip.'&lang='.$language->language);
    $form = array();
    /* enable one shop clicking */
    if ($one_shop_cc_enabled && !variable_get('novalnet_cc_cc3d') && !variable_get('novalnet_cc_force_cc3d') && arg(0) != 'admin') {
		
        $form['one_shop_cc_enabled'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array("id" => "one_shop_cc_enabled"),
            '#default_value'    => $one_shop_cc_enabled
        );
        $form['one_page_shopping'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array("id" => "one_page_shopping"),
            '#default_value'    => isset($enable_one_shop) ? $enable_one_shop : '1',
        );
        $form['enable_cc_one_shop'] = array(
            '#type'       => 'markup',
            '#attributes' => array('class' => 'enable_cc_one_shop'),
            '#markup'     => '<span class="cc_enter_new">' . t('Enter new card details') . '</span>',
        );
        /* masking pattern */
        $form['given_cc_details'] = array(
            '#type'   => 'markup',
            '#markup' => "<div class='given_cc_details' style=>
                    <table style='width: 50%;'><tr><td>" . t('Type of card') . "</td><td>" . $card_details['cc_card_type'] . " </td></tr><tr><td>" . t('Card holder name') . "</td><td>" . $card_details['cc_holder'] . " </td></tr>
                    <tr><td>" . t('Card number') . "</td><td>" . $card_details['cc_no'] . " </td></tr>
                    <tr><td>" . t('Expiry date') . "</td><td>" . $card_details['cc_exp_month'] . "/" . $card_details['cc_exp_year'] . " </td></tr></table>
                    </div>",
            '#element_validate' => array('uc_novalnet_nnccform_validate'),
        );
    }
    $form['nn_cc_hash_generated'] = array(
        '#type'           => 'hidden',
        '#default_value'  => !empty($card_details) ? 1 : 0,
        '#attributes'     => array('id' => 'nn_cc_hash_generated'),
    );
    $form['cc_one_div_first'] = array('#type' => 'markup', '#markup' => '<div id="loading_image"><img src="' . url(drupal_get_path('module', 'uc_novalnet'), array('relative' => TRUE)) . '/img/novalnet_loader.gif" /></div><div id="load_cc_iframe" style="display:block">');
    $form['enable_cc_iframe_shop'] = array(
        '#type'       => 'markup',
        '#attributes' => array('class' => 'enable_cc_iframe_shop', 'id' => 'enable_cc_iframe_shop'),
        '#markup'     => '<iframe width="100%" id="nnIframe" frameborder="0" scrolling="no" onload="load_cc_iframe()" src="https://secure.novalnet.de/cc?api=' . $fid  .'"></iframe>',
        '#element_validate' => array('uc_novalnet_nnccform_validate'),
    );
    $form['cc_one_div_last'] = array('#type' => 'markup', '#markup' => '</div>');
    $form['nn_label'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array("id" => "nn_label"),
        '#default_value'    => variable_get('common_label_style') ? variable_get('common_label_style') : '',
    );
    $form['nn_input'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array("id" => "nn_input"),
        '#default_value'    => variable_get('common_field_style') ? variable_get('common_field_style') : '',
    );
    $form['nn_css_text'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array("id" => "nn_css_text"),
        '#default_value'    => variable_get('common_style_text') ? variable_get('common_style_text') : '',
    );
    $form['nn_cc_hash'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array("id" => "nn_cc_hash"),
        '#default_value'    => "",
    );
    $form['novalnet_cc_uniqueid'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array("id" => "novalnet_cc_uniqueid"),
        '#default_value'    => "",
        '#element_validate' => array('uc_novalnet_nnccform_validation'),
    );

    return $form;
}


