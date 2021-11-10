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
 * Script   : uc_novalnet.inc
 *
 * This file contains the Novalnet payment handling functions
 * to complete the order purchase in the shop system.
 *
 */

    module_load_include('inc', 'uc_novalnet', '/includes/uc_novalnet_form');

/**
 * Payment method callback function: To get Novalnet payment methods and its information
 * $payment_methods - array that contains individual payment name, payment key and payment description.
 * Insert a new payment method here
 *
 * @param  string $nn_payment
 * @return array or string
 */
function uc_novalnet_payment_methods($nn_payment = '') {
    /* Novalnet payment method array */
    $payment_methods = array(
        'novalnet_cc'       => array(
                'name'      =>  t('Credit Card'),
                'key'       => '6',
                'link'      => t('https://www.novalnet.com/credit-card'),
                'desc'      => t('The amount will be debited from your credit card once the order is submitted')),
        'novalnet_ideal'    => array(
                'name'      => 'iDEAL',
                'key'       => '49',
                'link'      => t('https://www.novalnet.com/ideal'),
                'desc'      => sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
        'novalnet_sepa'     => array(
                'name'      => t('Direct Debit SEPA'),
                'key'       => '37',
                'link'      => t('https://www.novalnet.com/sepa-direct-debit'),
                'desc'      => t('Your account will be debited upon the order submission'),
                'guarantee_key' => '40'
                ),
        'novalnet_invoice'  => array(
                'name'      => t('Invoice'),
                'key'       => '27',
                'link'      => t('https://www.novalnet.com/invoice'),
                'desc'      =>  t("Once you've submitted the order, you will receive an e-mail with account details to make payment"),
                'guarantee_key' => '41'),
        'novalnet_paypal'   => array(
                'name'      => 'PayPal',
                'key'       => '34',
                'link'      => t('https://www.novalnet.com/paypal'),
                'desc'      =>  sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
        'novalnet_eps'      => array(
                'name'      => 'eps',
                'key'       => '50',
                'link'      => t('https://www.novalnet.com/eps-online-payment'),
                'desc'      =>  sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
        'novalnet_instantbank' => array(
                'name'         => t('Instant Bank Transfer'),
                'key'          => '33',
                'link'      => t('https://www.novalnet.com/online-instant-transfer'),
                'desc'         => sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
        'novalnet_prepayment' => array(
                'name'        => t('Prepayment'),
                'key'         => '27',
                'link'      => t('https://www.novalnet.com/prepayment'),
                'desc'        =>  t("Once you've submitted the order, you will receive an e-mail with account details to make payment")),
        'novalnet_giropay'    => array(
                'name'        => t('giropay'),
                'key'         => '69',
                'link'      => t('https://www.novalnet.com/giropay'),
                'desc'        =>  sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
        'novalnet_cashpayment'    => array(
                'name'        => t('Barzahlen'),
                'key'         => '59',
                'link'      => t('https://www.novalnet.com/barzahlen'),
                'desc'        =>  sprintf(t('After completing your order you get a payment slip from Barzahlen that you can easily print out or have it sent via SMS to your mobile phone. With the help of that payment slip you can pay your online purchase at one of our retail partners (e.g. supermarket).'), '<br/>')),
        'novalnet_przelewy24' => array(
                'name'        => t('Przelewy24'),
                'key'         => '78',
                'link'      => t('https://www.novalnet.com/przelewy24'),
                'desc'        =>  sprintf(t('After the successful verification, you will be redirected to Novalnet secure order page to proceed with the payment %s Please don’t close the browser after successful payment, until you have been redirected back to the Shop'), '<br/>')),
    );
    /* if payment name passed as empty, it will returns entire payment array otherwise returns the individual payment array */
    return (!empty($nn_payment)) ? $payment_methods[$nn_payment] : $payment_methods;
}

function uc_novalnet_logo_url($key) {
	switch ($key) {
        case 'novalnet_cc':
          return t('https://www.novalnet.com/credit-card');
        case 'novalnet_sepa':
          return t('https://www.novalnet.com/sepa-direct-debit');
        case 'novalnet_invoice':
          return t('https://www.novalnet.com/invoice');
        case 'novalnet_prepayment':
          return t('https://www.novalnet.com/prepayment');
        case 'novalnet_cashpayment':
          return t('https://www.novalnet.com/barzahlen');
        case 'novalnet_ideal':
			return t('https://www.novalnet.com/ideal');
        case 'novalnet_instantbank':
          return t('https://www.novalnet.com/online-instant-transfer');
        case 'novalnet_paypal':
          return t('https://www.novalnet.com/paypal');
        case 'novalnet_eps':
			return t('https://www.novalnet.com/eps-online-payment');
        case 'novalnet_giropay':
          return t('https://www.novalnet.com/giropay');
        case 'novalnet_przelewy24':
          return t('https://www.novalnet.com/przelewy24');
    }
}

/**
 *
 * Payment method callback function: To get the payport url for relevant requests
 *
 * $request_type - based on that request type, process url is retrived for the payment process.
 *
 * @param  string $request_type
 * @return string
 */
function uc_novalnet_get_url($request_type) {
    switch ($request_type) {
		
        /* Get the payment url for cURL process */
        case 'nncurl_request':
          return 'https://payport.novalnet.de/paygate.jsp';
        /* Get the payment url for xml process */
        case 'nnxml_request':
          return 'https://payport.novalnet.de/nn_infoport.xml';
        /* Request for auto api configuration call */
        case 'novalnet_auto_config':
          return 'https://payport.novalnet.de/autoconfig';
        /* Get the payment url for redirection payments */
        case 'novalnet_ideal':
        case 'novalnet_instantbank':
          return 'https://payport.novalnet.de/online_transfer_payport';
        case 'novalnet_paypal':
          return 'https://payport.novalnet.de/paypal_payport';
        case 'novalnet_eps':
        case 'novalnet_giropay':
          return 'https://payport.novalnet.de/giropay';
        case 'novalnet_cc_pci':
          return 'https://payport.novalnet.de/pci_payport';
        case 'novalnet_przelewy24':
          return 'https://payport.novalnet.de/globalbank_transfer';
    }
}

/**
 *
 * menu callback : Display the admin portal inside the shop
 *
 * Merchant login into admin portal in shop itself to activate the payment methods
 *
 * @param  none
 * @return none
 */
function uc_novalnet_merchant_admin() {
    $support = t('Login here with Novalnet merchant credentials. For the activation of new payment methods please contact %ssupport@novalnet.de%s');
    echo '<div style="background-color: #0080c9;color: #fff;font-family: calibri;font-size: 16px;font-weight: bold;padding: 5px;text-align: center;">' . sprintf($support, "<a href='mailto:sales@novalnet.de' style='color: #fff'>", "</a>") . '</div><iframe frameborder="0" style="width:100%;height:80%;" src="https://admin.novalnet.de"></iframe>';
    exit;
}

/**
 *
 * Payment method sanitize function: sanitize the refer id parameters
 *
 * Remove the unwanted tags and spaces from the referer id
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_sanitize_referid($element, &$form_state) {
    $form_state['values'] = array_map('strip_tags', $form_state['values']);
    $form_state['values'] = array_map('trim', $form_state['values']);
}

/**
 *
 * Payment method validate function for number validation.
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_number($element, &$form_state)  {
    $element_value = trim($element['#value']);
    if ( $element_value != '') {
        /* Numeric validation - Throws a message, if it is not contains numeric values */
        if (!preg_match("/^[0-9]+$/", $element_value)) {
            /* Varibale that contains the error message with field name(E.g: Telephone number is not valid) */
            $errormsg = $element['#title'] . ' ' . t('is invalid');
            form_error($element, check_plain($errormsg));
        }
    }
}

/**
 *
 * Payment method validate function: to validate the mobile number.
 *
 * $payment - To get the current payment name
 *
 * $payment_details - array that contains the form values for the current payment
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_fraud_pin($element, &$form_state) {
    $payment         = $form_state['values']['panes']['payment']['payment_method'];
    $payment_details = $form_state['values']['panes']['payment']['details'];

    /* Variable that contains the error message with field name(E.g: Telephone number is not valid) */
    $errormsg = str_replace('*', '', $element['#title'] . ' ' . t('is invalid'));

    /* Quit the form validation while using payment_ref in CC payment */
    if (!empty($payment_details['one_shop_cc_enabled']))
        return TRUE;

    /* Quit the form validation while using payment_ref in SEPA payment */
    if (!empty($payment_details['one_shop_sepa_enabled']) && empty($payment_details['sepa_given_details']))
        return TRUE;

    /* Sanitize the pin element */
    if (empty($payment_details[$payment . '_pinkey']))
        form_error($element, check_plain(strip_tags($errormsg)));
    /* Validation for numbers and mail */
    if (trim($element['#value']) != '') {
        if (($element['#attributes']['id'] == 'fraud_number' && !preg_match("/^[0-9]+$/", trim($element['#value'])))) {
            form_error($element, check_plain(strip_tags($errormsg)));
        } else {
            /* stores the pin value in novalnet payment session */
            uc_novalnet_setsession($payment, array($payment . '_pinkey'  => trim($element['#value'])));
        }
    }
}

/**
 *
 * Payment method callback function: to complete the order purchase for redirection payment
 *
 * Function is used to complete the order and save the resposne log
 *
 * $parsed - variable that contains the server response
 *
 * @param  none
 * @return none
 */
function uc_novalnet_nnpayment_complete() {
    $parsed = $_REQUEST; unset($_POST);
    $parsed['order_no'] = !empty($parsed['order_no']) ? $parsed['order_no'] : $parsed['inputval3'] ;
    /* To save the payment response in Novalnet transaction logs */
    uc_novalnet_save_log($parsed['order_no'], $parsed, 'response');
    /* To complete the payment transaction */
    uc_novalnet_completeorder($parsed);
}

/**
 *
 * Payment method validate function: to validate the pin number
 *
 * $payment - To get the current payment name
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_pin($element, &$form_state) {
    /* Fetch the Payment method name from the form_state values */
    $payment = $form_state['values']['panes']['payment']['payment_method'];
    $errormsg = t('Pin number is not valid');
    $pin_value = trim($element['#value']);
    /* Quit the pin validation if it contains the empty value */
    if (!$form_state['values']['panes']['payment']['details'][$payment . '_forget_pin']) {
        /* Pin validation - if it contains special characters or spaces */
        if (preg_match('/[\'\/~`\!@#\$%\^&\*\(\)_\-\+=\{\}\[\]\|;:"\<\>,\.\?\\\]/', $pin_value)) {
          $errormsg = t('The PIN you entered is incorrect');
          form_error($element, check_plain($errormsg));
        }
        elseif (empty($pin_value)) {
            $errormsg = t('Enter your PIN');
            form_error($element, check_plain($errormsg));
        }
        /* Stores the pin value in session and forget pin flag set as empty (0) */
        uc_novalnet_setsession($payment, array('pin'  => trim($element['#value']), 'forget_pin' => 0));
    }
    else {
        /* store the forget pin flag as 1, when end-user hits the forget pin */
        uc_novalnet_setsession($payment, array('forget_pin'  => trim($form_state['values']['panes']['payment']['details'][$payment . '_forget_pin'])));
    }
}

/**
 *
 * Function to validate the basic and customer related parameters for payment process from admin  create order
 *
 * @param  object $order
 * @return none
 */
function uc_validate_novalnet_getparams($order) {
    $is_error = FALSE;
    /* Validating customer details for order process */
    foreach (array('First name' => 'first_name', 'Last name' => 'last_name', 'Street' => 'street1', 'City' => 'city', 'Zone' => 'zone', 'Postal code' => 'postal_code', 'Country' => 'country') as $params => $values) {
          $parameters = 'billing_' . $values;
        if (empty($order->$parameters)) {
            $is_error = TRUE;
            drupal_set_message('Basic parameter ' . $params . ' is invalid', 'error');
        }
    }

    /* Validating customer e-mail address*/
    if (empty($order->primary_email)) {
        $is_error = TRUE;
        drupal_set_message(t('Basic parameter email is invalid'), 'error');
    }
    if ($is_error)
        /* checking whether admin placed re-order */
        if (isset($_SESSION['cart_order']))
          drupal_goto('cart/checkout');
        else
            drupal_goto('admin/store/orders/' . $order->order_id . '');
}
/**
 *
 * Payment method callback function: To form the basic and customer related parameters for payment process
 *
 * @param  array $data
 * @param  object $order
 * @return none
 */
function uc_novalnet_getparams(&$data, $order) {
    global $language;
    $config_data     = basic_params();
    $billing_country = uc_get_country_data(array('country_id' => $order->billing_country));
    $ipaddr          = ip_address();
    /* Fetching the ubercart version details */
    $ubercart_info = system_get_info('module', 'uc_payment');
    $info_version = explode('-', $ubercart_info['version']);
    /* Fetching the payment details */
    $payment_details = uc_novalnet_payment_methods($order->payment_method);
    /* Form the payment call parameters */
    $data = array(
                'vendor'           => $config_data['vendor_id'],
                'product'          => $config_data['product_id'],
                'tariff'           => $config_data['tariff_id'],
                'auth_code'        => $config_data['auth_code'],
                'test_mode'        => variable_get($order->payment_method . '_test_mode', 0),
                'currency'         => variable_get('uc_currency_code', 'USD'),
                'first_name'       => trim($order->billing_first_name),
                'last_name'        => trim($order->billing_last_name),
                'gender'           => 'u',
                'email'            => trim($order->primary_email),
                'street'           => $order->billing_street1,
                'search_in_street' => 1,
                'city'             => $order->billing_city,
                'zip'              => $order->billing_postal_code,
                'tel'              => trim($order->billing_phone),
                'lang'             => strtoupper($language->language),
                'language'         => strtoupper($language->language),
                'country_code'     => $billing_country[0]['country_iso_code_2'],
                'country'          => $billing_country[0]['country_iso_code_2'],
                'session'          => session_id(),
                'customer_no'      => ($order->uid > 0 ) ? $order->uid : 'guest',
                'remote_ip'        => ( filter_var($ipaddr, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6) || $ipaddr == '::1' ) ? '127.0.0.1' : $ipaddr,
                'order_no'         => $order->order_id,
                'payment_method'   => $order->payment_method,
                'system_name'      => 'drupal-ubercart',
                'system_version'   => VERSION . '-' . $info_version['1'] . '-NN11.2.0',
                'system_ip'        => ( filter_var($_SERVER['SERVER_ADDR'], FILTER_VALIDATE_IP, FILTER_FLAG_IPV6) || $_SERVER['SERVER_ADDR'] == '::1' ) ? '127.0.0.1' : $_SERVER['SERVER_ADDR'],
                'system_url'       => url('', array('absolute' => TRUE)),
                'amount'           => round($order->order_total, 2) * 100,
                'key'              => $payment_details['key'],
            );
    /* Company parameter added for payment call */
      if (!empty($order->billing_company)) {
        $data['company'] = $order->billing_company;
      }
    /* Checking the possibilities for guarantee payments */
    if (variable_get($order->payment_method . '_payment_guarantee_check', 0)) {
		uc_novalnet_check_guarantee_payments($data, $order);
	}
    /* Checking for reference payment payment_ref = TID */
    
    if ((!empty($_SESSION['novalnet_session'][$order->payment_method]['ref']) && variable_get($order->payment_method . '_enable_one_shop_clicking') == 1)) {
        $transaction_id = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('tid'), array('is_ref_order' => '1'), array('paymentkey' => $data['key']));
        $data['payment_ref'] = $transaction_id['tid'];
        if($order->payment_method == 'novalnet_cc' && !empty($_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_uniqueid']) && !empty($_SESSION['novalnet_session'][$order->payment_method]['nn_cc_hash'])) 
			unset($data['payment_ref']);
    }

    /* Manual check limit for allowed payments */
    if (in_array($order->payment_method, array('novalnet_cc', 'novalnet_invoice', 'novalnet_sepa', 'novalnet_paypal')) && variable_get($order->payment_method . '_enable_one_shop_clicking') != 2)
        uc_novalnet_checklimit($data, $order);
      if ($data['key'] != '40') {
         /* checking the possibilities for zero amount booking */
          uc_novalnet_zero_amount_booking($data, $order->payment_method, $config_data);
      }

    /* Load the individual payment params */
    if (!isset($_SESSION['novalnet_session'][$order->payment_method]['tid']))
        uc_novalnet_additional_params($data, $order, $config_data['payment_access_key']);
    if ($order->payment_method == 'novalnet_cc') {
        $_SESSION['novalnet_session'][$order->payment_method]['cc_request'] = $data;
        $data['nn_it'] = 'iframe';
        if (!empty($_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_uniqueid']))
            $data['unique_id'] =  $_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_uniqueid'];
        if (!empty($_SESSION['novalnet_session'][$order->payment_method]['nn_cc_hash']))
            $data['pan_hash'] =  $_SESSION['novalnet_session'][$order->payment_method]['nn_cc_hash'];
    }
    /* Send the notify url on payment call, which is not empty */
    if (!empty($config_data['notify_url']))
        $data['notify_url'] = $config_data['notify_url'];
        
    if ( in_array($order->payment_method, array('novalnet_paypal', 'novalnet_cc', 'novalnet_sepa')) && (variable_get($order->payment_method . '_enable_one_shop_clicking', 0) != 0 && empty($data['payment_ref'])) && arg(0) != 'admin' && empty($_SESSION['novalnet_session'][$order->payment_method]['gurantee']) && ($order->uid != 0 || $order->uid == 0 && variable_get($order->payment_method . '_enable_one_shop_clicking') == 2)) {
        $data['create_payment_ref'] = 1;
    }
    if ($order->payment_method == 'novalnet_cc') {
        $enable_one_shop = variable_get($order->payment_method . '_enable_one_shop_clicking') == 1 ? TRUE : FALSE;
        if ($order->uid != 0) {
            $card_details    = nn_variable_get(array('customer_id' => $order->uid), 'uc_novalnet_transaction', array('masked_acc_details'), array('is_ref_order' => '1'), array('paymentkey' => 6));
            if (!empty($card_details))
                $card_details = unserialize($card_details['masked_acc_details']);
        }
        $one_shop_cc_enabled = !empty($enable_one_shop) && !empty($card_details) ? 1 : 0;
        if ($one_shop_cc_enabled && variable_get('novalnet_cc_cc3d') && variable_get('novalnet_cc_force_cc3d')) unset($data['create_payment_ref']);
    }
   /* Send the second street name on payment call, which is not empty */
    if (!empty($order->billing_street2))
      $data['street'] .= ' ' . $order->billing_street2;
    /* Send the referer id on payment call, which is not empty */
    if (!empty($config_data['referrer_id']))
        $data['referer_id'] = $config_data['referrer_id'];
}

/**
 *
 * Payment method callback function: zero amount booking params for server call
 *
 * @param  array $data
 * @param  array $payment
 * @param  array $config_data
 * @return boolean
 */
function uc_novalnet_zero_amount_booking(&$data, $payment, $config_data) {
    if (isset($_SESSION['novalnet_session'][$payment]['zero_amount'])) 
        {
            unset($_SESSION['novalnet_session'][$payment]['zero_amount']);
        }
    if (variable_get($payment . '_enable_one_shop_clicking') == 2 && in_array($payment, array('novalnet_cc', 'novalnet_sepa', 'novalnet_paypal')))
    {
        $_SESSION['novalnet_session'][$payment]['zero_amount'] = $data['amount'] = 0;
    } # add allowed payments
        
    return isset($_SESSION['novalnet_session'][$payment]['zero_amount']);

}

/**
 *
 * Payment method callback function: to complete Novalnet order purchase in shop
 *
 * @param  array $parsed
 * @param  array $order
 * @return none
 */
function uc_novalnet_completeorder($parsed = array(), $order = array()) {
    global $language;
    $config_data = basic_params();
    /* Supported payments for redirection payments */
    $redirection_array = array('novalnet_paypal', 'novalnet_ideal', 'novalnet_instantbank', 'novalnet_eps', 'novalnet_giropay', 'novalnet_przelewy24');
    $parsed['order_no'] = !empty($parsed['order_no']) ? $parsed['order_no'] : $parsed['inputval3'] ;
    /* To check the payment status from the server reposnse */
    $status_message = uc_novalnet_check_status($parsed);
    /* If order object is empty - load the order from order-number */
    if (empty($order)) $order = uc_order_load($parsed['order_no']);
        $parsed['payment_method'] = $order->payment_method;
        $redirect_test = $redirection_array;
    if ($order->payment_method == 'novalnet_cc' && variable_get($order->payment_method . '_cc3d') == 1) {
        $redirect_test[7] = 'novalnet_cc';
    }
    if ($status_message != 'success') {
        drupal_set_message(check_plain($status_message), 'error');
        watchdog('uc_novalnet', '<br/> Payment Error: @error', array('@error' => $status_message), WATCHDOG_ERROR);
        /* checking whether admin placed re-order */
        if (isset($_SESSION['cart_order']))
          drupal_goto('cart/checkout');
        else
          drupal_goto('admin/store/orders/' . $parsed['order_no']);
    }

    if ($parsed['payment_method'] == 'novalnet_cc' && isset($_SESSION['novalnet_session'][$order->payment_method])) unset($_SESSION['novalnet_session'][$order->payment_method]['nn_cc_hash'], $_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_uniqueid']);
        if (variable_get('novalnet_paypal_enable_one_shop_clicking') == 1 && isset($_SESSION['novalnet_session']['novalnet_paypal']['paypal_given_details']) && $_SESSION['novalnet_session']['novalnet_paypal']['paypal_given_details'] == 1)
            unset($redirect_test[0]);
    /* Checks the payment is redirection, if it is redirection payment which is allowed for check hash process and decoding the params from response */
    if (in_array($parsed['payment_method'], $redirect_test)) {
        /* Check hash process - Using payment response and payment access key from the shop back-end */
        uc_novalnet_checkHash($parsed, $config_data['payment_access_key']);
        /* Decoding process - Using payment response and payment access key from the shop back-end */
        uc_novalnet_decode_param($parsed);
    }
    /* form the payment comments to display in the order success page as well as for emails and invoices */
    $parsed['nn_comments'] = uc_novalnet_get_comments($parsed);

    if (isset($parsed['product_id'])) $parsed['product'] = $parsed['product_id'];
    // Function to complete the order in shop
    /* Set before order status for the Invoice, prepayment and paypal 90 */
    if (((in_array($parsed['payment_method'], array('novalnet_invoice', 'novalnet_prepayment', 'novalnet_cashpayment'))) && (!in_array($parsed['key'], array(40,41)) && ($parsed['tid_status'] == 100 ))) || ($parsed['tid_status'] == 90 && $parsed['payment_method'] == 'novalnet_paypal') || ($parsed['tid_status'] == 86 && $parsed['payment_method'] == 'novalnet_przelewy24')) {
        $order->order_total = 0;
        $order_status = variable_get($parsed['payment_method'] . '_before_status', 'Pending');
        
    } else {
		 $order_status = variable_get($parsed['payment_method'] . '_after_status', 'completed');
    }
    if (in_array($parsed['tid_status'], array('85', '91', '98', '99'))) {
        $order_status = $config_data['onhold_transaction_status'];
    } 
    /* Set order completion status */
    if(in_array($parsed['key'], array(40,41)) && ($parsed['tid_status'] == 75 )) $order_status = variable_get($parsed['payment_method'] . '_guarantee_before_status', 'completed');
    
    
    /* Updating the order comments */
    uc_order_comment_save($order->order_id,  0, $parsed['nn_comments'] ,  'order', $order_status , 'TRUE');

    /* Adding the line items in order history */
    uc_payment_enter($order->order_id,  $parsed['payment_method'],  $order->order_total,  $order->uid,  NULL,  $parsed['nn_comments']);

    /* Updatig the order status */
    uc_order_update_status($order->order_id, $order_status);

    /* Cashpayment UI page generation*/
    if ($parsed['payment_method'] == 'novalnet_cashpayment' && $parsed['cp_checkout_token']) {
        if ($parsed['test_mode'] == '1') {
            $url = 'https://cdn.barzahlen.de/js/v2/checkout-sandbox.js';
        } else {
            $url = 'https://cdn.barzahlen.de/js/v2/checkout.js';
        }
        $checkout_token = $parsed['cp_checkout_token'];
        $parsed['nn_comments'] .= "<script src='$url' data-token='$checkout_token' class='bz-checkout'></script><button id='barzahlen_button' class='bz-checkout-btn'>".t('Pay now with Barzahlen')."</button>";
    }

    drupal_set_message($parsed['nn_comments']);

    /* To update the transaction in Novalnet transaction table */
    uc_novalnet_update_status($parsed);

    $_SESSION['do_complete'] = TRUE;
    uc_novalnet_setsession();
    /* checking whether admin placed re-order or end-user - based on that return url differs */
    if (isset($_SESSION['cart_order'])) {
        $_SESSION['uc_checkout'][$_SESSION['cart_order']]['do_complete'] = TRUE;
        drupal_goto('cart/checkout/complete');
    } else
        drupal_goto('admin/store/orders/' . $parsed['order_no']);
}

/**
 *
 * Payment method callback function: For onhold product
 *
 * @param  array $data
 * @param int $order
 * @return none
 */
function uc_novalnet_checklimit(&$data, $order) {
    if(variable_get($order->payment_method. '_paymentoption') == 'capture') {
      return TRUE;
	}
  // If the merchant have configure the manual check amount and the amount is lesser than order amount then add on_hold param.
  if (variable_get($order->payment_method. '_paymentoption') == 'authorize' && $data['amount'] >= variable_get($order->payment_method . '_manualamount')) {
    $data['on_hold'] = 1;
  }
}

/**
 *
 * Payment method callback function: To form the payment parameters
 *
 * Use this function to add specific params(Like implemetation, due_date etc.) for individual payments.
 *
 * @param  array $data
 * @param  object $order
 * @param int $key
 * @return none
 */
function uc_novalnet_additional_params(&$data, $order, $key) {
    global $base_url, $user;
    $data['input3']           = 'order_id';
    $data['inputval3']        = $order->order_id;
    switch ($order->payment_method) {
        /* Adding payment params(hash, user_variable, unique id and encoded values) for redirect payments */
        case 'novalnet_cc':
            /* CC 3d flas is enabled */
            if (variable_get('novalnet_cc_cc3d')) {
                $data['cc_3d'] = 1;
				$data['uniqid'] = randomStringHash();
				$data['implementation'] = 'ENC';
				uc_novalnet_encode_data($data);
				$data['hash'] = uc_novalnet_create_hash($data);
				$data['user_variable_0'] = $base_url;
            }
            if (empty($_SESSION['novalnet_session']['novalnet_cc']['ref'])) {
                unset($data['payment_ref']);
            }
        break;
        case 'novalnet_ideal':
        case 'novalnet_eps':
        case 'novalnet_giropay':
        case 'novalnet_instantbank':
        case 'novalnet_paypal':
        case 'novalnet_przelewy24':
            if (variable_get($order->payment_method . '_enable_one_shop_clicking') == 2)
            $_SESSION['novalnet_session'][$order->payment_method]['request'] = serialize($data);

                $data['uniqid'] = randomStringHash();
                /* Formation of encoded params for redirect payments */
                uc_novalnet_encode_data($data);
                $data['hash'] = uc_novalnet_create_hash($data);
                $data['user_variable_0'] = $base_url;
                $data['implementation']  = 'ENC';
        break;
        /* Adding specific(iban, bank_account_holder) params for SEPA */
        case 'novalnet_sepa':
            /* Fetch the sepa due date from the shop back-end */
            if (variable_get($order->payment_method . '_due_date') != '' && variable_get($order->payment_method . '_due_date') >= 2 && variable_get($order->payment_method . '_due_date') <= 14) {
                $due_date = variable_get($order->payment_method . '_due_date');
                $data['sepa_due_date']        = date('Y-m-d', strtotime("+ $due_date day"));
            }
            if (empty($data['payment_ref'])) {
                $data['bank_account_holder']  = isset($_SESSION['novalnet_session']['novalnet_sepa']['nnsepa_owner']) ? $_SESSION['novalnet_session']['novalnet_sepa']['nnsepa_owner'] : '';
                $data['iban']  = isset($_SESSION['novalnet_session']['novalnet_sepa']['nnsepa_iban']) ? $_SESSION['novalnet_session']['novalnet_sepa']['nnsepa_iban'] : '';
            }
        break;
        /* Adding specific(invoice_ref, invoice_type and due_date) params for Invoice */
        case 'novalnet_invoice':
            $data['invoice_ref']    = "BNR-" . $data['product'] . "-" . $data['order_no'];
            $data['invoice_type']   = 'INVOICE';
            $due_date = (variable_get($order->payment_method . '_due_date')=='')? 14 : variable_get($order->payment_method . '_due_date');
            $data['due_date']     = date('Y-m-d', strtotime("+ $due_date day"));
        break;
        /* Adding specific(invoice_ref, invoice_type) params for prepayment */
        case 'novalnet_prepayment':
            $data['invoice_ref']    = "BNR-" . $data['product'] . "-" . $data['order_no'];
            $data['invoice_type']   = 'PREPAYMENT';
        break;
        case 'novalnet_cashpayment':
			$due_date = (variable_get($order->payment_method . '_due_date')=='')? 14 : variable_get($order->payment_method . '_due_date');
            $data['cp_due_date']     = date('Y-m-d', strtotime("+ $due_date day"));
        break;
    }
}

/**
 *
 * Payment method callback function: unique-id generation function
 *
 * @param none
 * @return string
 */
function randomStringHash($length = 16)
{
    $randomArray = ['8','7','6','5','4','3','2','1','9','0','9','7','6','1','2','3','4','5','6','7','8','9','0'];
    shuffle($randomArray);
    return substr(implode($randomArray, ''), 0, $length);
}

/**
 *
 * Payment method callback function: set encoded params for relevant payments
 *
 * @param  array $data
 * @param int $key
 * @return none
 */
function uc_novalnet_encode_data(&$data) {
    $encode_data = array();
    $encode_data = array('auth_code','product','tariff','amount','test_mode');
    foreach ($encode_data as $value) {
        if (isset($data[$value])) {
            $data[$value] = encrypt($data[$value], $data['uniqid']);
        }
    }
}

/**
 * @param  array $input
 * @param  array $salt
 *
 * @return string
 */
function encrypt($input, $salt)
{
    $config_data     = basic_params();
    // Return Encrypted Data.
    return htmlentities(base64_encode(openssl_encrypt($input, "aes-256-cbc", $config_data['payment_access_key'], true, $salt)));
}

/**
 *
 * Payment method callback function: Set decode params
 *
 * @param array $data
 * @return none
 */
function uc_novalnet_decode_param(&$data) {
    $params = array('auth_code','product','tariff','amount','test_mode');
    foreach ($params as $value) {
        if (isset($data[$value])) {
            $data[$value] = decrypt($data[$value], $data['uniqid']);
        }
    }
}

/**
 * @param  array $input
 * @param  array $salt
 *
 * @return string
 */
function decrypt($input, $salt)
{
    $config_data     = basic_params();
    // Return decrypted Data.
    return openssl_decrypt(base64_decode($input), "aes-256-cbc", $config_data['payment_access_key'], true, $salt);
}

/**
 *
 * Payment method callback function: hash generation
 *
 * @param array $data
 * @return string
 */
function uc_novalnet_create_hash($data) {
    $basic_data                    = basic_params();
    $hash_string = array('auth_code','product','tariff','amount','test_mode','uniqid');
    $string = '';
    foreach ($hash_string as $param) {
        $string .= $data[$param];
    }
    $string .= strrev($basic_data['payment_access_key']);
    return hash('sha256', $string);
}

/**
 *
 * Payment method callback function: to check the payment hash
 *
 * @param array $parsed
 * @param int $key
 * @return bool
 */
function uc_novalnet_checkHash($parsed, $key) {
    /* Check the hash2 values with generated hash values */
    if ($parsed['hash2'] != uc_novalnet_create_hash($parsed)) {
        drupal_set_message(t('While redirecting some data has been changed. The hash check failed.'), 'error');
        drupal_goto('cart/checkout');
    }
}

/**
 *
 * Payment method callback function: to check the payment status
 *
 * @param array $response
 * @return string
 */
function uc_novalnet_check_status($response) {
    /* Check the status i.e. recieved form the response */
    if (isset($response['status'])) {
        if ($response['status'] == 100 || ($response['status'] == 90 &&  $response['payment_method'] == 'novalnet_paypal')) return 'success';
        elseif (isset($response['status_text'])) return $response['status_text'];
        elseif (isset($response['status_desc'])) return  $response['status_desc'];
        elseif (isset($response['novaltel_status_message'])) return  $response['novaltel_status_message'];
        elseif (isset($response['status_message'])) {
            if ($response['status'] == '0529006') {
            $_SESSION['novalnet_error_payment'] = array($response['payment_method'] => array('error_time' => time()));
            }
            return  $response['status_message'];
        }
    } /* Check the pin status */
    elseif (isset($response['pin_status']))
        return $response['pin_status']['status_message'];
}

/**
 *
 * Payment method callback function: to append the payment comments in order success page, mail and invoices
 *
 * @param array $data
 * @param object $new_order
 * @param array $tid_details
 * @return string $comment
 */
function uc_novalnet_get_comments($data, $new_order='', $tid_details='') {
    /* Checking the test mode from the response */
    $test_mode = ($data['test_mode'] == 1) ? t('Test order') : '';
    /* Load the payment array using payment response */
    $payment   = uc_novalnet_payment_methods($data['payment_method']);
    if (!empty($tid_details)) {
        $data['invoice_ref']    =  "BNR-" . $tid_details['product_id'] . "-" . $tid_details['order_id'];
    }
    /* Check if it is a new order generated for recurring, from the callback-script */
    if (!empty($new_order)) {
    /* Get the Basic parameters */
        $basic_data = basic_params();
        $data['order_no']       = $new_order->order_id;
        $data['amount']         = $data['amount']/100;
        $data['invoice_ref']    =  "BNR-" . $basic_data['product_id'] . "-" . $data['order_no'];
    }
    $comment = '';
    if (!empty($data['payment_id']) && in_array($data['payment_id'], [41,40]) && in_array($data['tid_status'], [75,100])) {
        $comment .= t('This is processed as a guarantee payment'). '<br />';        
    }
    
    $comment   .= $payment['name'] . '<br />' .  strip_tags(t('Novalnet transaction ID%s', array('%s' => ':'))) . ' ' . '<span id="tid_update">' . $data['tid'] . '</span><br />' . $test_mode . '<br />';

    /* Comments formation for Invoice and Prepayment */
    if (in_array($data['payment_method'], array('novalnet_invoice', 'novalnet_prepayment')) || (!empty($data['invoice_det']))) {		 
        
        if ($data['tid_status'] == '75') {
            $comment .= t('Your order is under verification and once confirmed, we will send you our bank details to where the order amount should be transferred. Please note that this may take upto 24 hours.'). '<br />';
        }
            /* To remove the account details from the invoice mail after full amount paid to Novalnet */
        $nn_prepayment_comments = ($data['payment_method'] == 'novalnet_prepayment') ? '<div id=nn_prepayment_comments>' : '';
        if ($data['tid_status'] != '75') {
            $comment .= $nn_prepayment_comments . t('Please transfer the amount to the below mentioned account details of our payment processor Novalnet') . '<br />';
            $comment .= t('Due date: @due_date', array('@due_date' => $data['due_date'])) . '<br/>' .
                        t('Account holder: @comp', array('@comp' => $data['invoice_account_holder'])) . '<br/>' .
                        t('IBAN: @invoice_iban', array('@invoice_iban' => $data['invoice_iban'])) . '<br/>' .
                        t('BIC: @invoice_bic', array('@invoice_bic' => $data['invoice_bic'])) . '<br/>' .
                        t('Bank: @bank', array('@bank' => $data['invoice_bankname'] . ' ' . $data['invoice_bankplace'])) . '<br/><span id="amount_update">' .
                        t('Amount: @amount', array('@amount' => uc_currency_format($data['amount']))) . '</span><br />';
            /* Checking the new order flag */
            if (empty($new_order))
            $comment .= t('Please use any one of the following references as the payment reference, as only through this way your payment is matched and assigned to the order:') . '<br/>';
            $comment .= t("Payment Reference 1") . ': ' . $data['invoice_ref'] . '<br/>';
            $comment .= t("Payment Reference 2") . ': ' . t('TID') . ' ' . $data['tid'] . '<br/>';
        }
    }
    
    if ($data['payment_method'] == 'novalnet_cashpayment') {
        $novalnet_slip_expiry_date = ($data['cp_due_date']) ? $data['cp_due_date'] : $novalnet_slip_expiry_date;
        if ($novalnet_slip_expiry_date) {
            $comment .= t('Slip expiry date') . ': ' . (($data['lang'] == 'EN') ? date('d M Y', strtotime($novalnet_slip_expiry_date)) : date('d.m.Y', strtotime($novalnet_slip_expiry_date))) .  '<br />';
        }
        $comment .=  '<br />' . t('Store(s) near you') . '<br />';
        $nearest_storecounts = 1;
        foreach ($data as $key => $value) {
            if (strpos($key, 'nearest_store_title') !== FALSE) {
                $nearest_storecounts++;
            }
        }
        for ($i = 1; $i < $nearest_storecounts; $i++) {
            $comment .= $data['nearest_store_title_' . $i] . '<br />';
            $comment .= $data['nearest_store_street_' . $i] . '<br />';
            $comment .= $data['nearest_store_city_' . $i] . '<br />';
            $comment .= $data['nearest_store_zipcode_' . $i] . '<br />';
            $comment .= $data['nearest_store_country_' . $i] . '<br />' . '<br />';
        }
    }
    
    if (!empty($data['payment_id']) && $data['payment_id'] == 40 && $data['tid_status'] == '75') {
			$comment .= t('Your order is under verification and we will soon update you with the order status. Please note that this may take upto 24 hours.'). '<br />';
		}
    return $comment;
}

/**
 *
 * Payment method callback function: To update the transaction status in Novalnet(uc_novalnet_transaction) table.
 *
 * @param array $data
 * @return none
 */
function uc_novalnet_update_status($data) {
    /* Get the Basic parameters */
    $basic_data = basic_params();
    /* fetching the original transaction details from uc_novalnet_transaction table */
    $transaction_details = $data;
    if (empty($data)) return $transaction_details;
    /* Load the order object */
    $order = uc_order_load($data['order_no']);
    $reference_order = 0;
    /* Setting the flag for referenced transaction */
    if (!empty($data['payment_id']) && in_array( $data['payment_id'], array( 6, 37, 40, 34, 41)))
    $reference_order = (empty($data['payment_ref']) && variable_get($data['payment_method'] . '_enable_one_shop_clicking') == 1 && !empty($data['create_payment_ref'])) ? 1 : 0;
    /* Get the payment request from the session */
    $request = !empty($_SESSION['novalnet_session'][$data['payment_method']]['request']) ? $_SESSION['novalnet_session'][$data['payment_method']]['request'] : '';

    /* Get the payment response from the session, during the procees of fraud preventions */
    if (!empty($_SESSION['novalnet_session'][$data['payment_method']]['masked_acc_details']))
    $transaction_details = $_SESSION['novalnet_session'][$data['payment_method']]['masked_acc_details'];
    /* Unset the pan_hash from the response */
    if (isset($transaction_details['pan_hash'])) unset($transaction_details['pan_hash']);

    /* Based on the server response amount updation in novalnet transaction table */
    $amount = ( isset($transaction_details['key']) && (in_array($transaction_details['key'], array(49, 50, 33, 34, 69, 78)) || ($transaction_details['key'] == 6 && (variable_get('novalnet_cc_cc3d') || variable_get('novalnet_cc_force_cc3d'))))) ? $transaction_details['amount'] : round($transaction_details['amount'], 2) * 100;
    $amount = empty($amount) ? 0 : $amount;
    /* Update the transactions in Novalnet transaction (uc_novalnet_transaction) table  */
    $update_data = array('tid' => $data['tid'], 'status' => $transaction_details['tid_status'], 'key' => $data['key'], 'amount' => $amount, 'request' => $request, 'masked_acc_details' => serialize($transaction_details), 'customer_id' => $order->uid, 'is_ref_order' => $reference_order, 'callback_total' => $amount);

    /* Unset the Novalnet payment session */
    unset($_SESSION['novalnet_session'][$data['payment_method']]);
    foreach (array('vendor_id', 'auth_code', 'product_id', 'tariff_id') as $key) {
        $update_data[$key] = $basic_data[$key];
    }
    /* Update the due date in Novalnet transaction */
    $update_data['due_date'] = ($data['key'] == 27 || $data['key'] == 41) ? date('Y-m-d H:i:s', strtotime($data['due_date'])) : date('Y-m-d H:i:s') ;

    /* update the transaction details in novalnet transaction (uc_novalnet_transaction) table */
    update_variable(array('order_id' => $data['order_no']), $update_data, 'uc_novalnet_transaction');

    /* update the transaction with affiliate, if it is activated */
    if (!empty($_SESSION['nn']['nn_aff_id'])) {
        if (!empty($_SESSION['nn']['update'])) {
            update_variable(array('aff_id' => $_SESSION['nn']['nn_aff_id']), array('aff_id' => $_SESSION['nn']['nn_aff_id'], 'customer_id' => $order->uid, 'aff_order_no' => $data['order_no']), 'nn_aff_user_detail');
        }
        unset($_SESSION['nn']);
    }
}

/**
 *
 * Payment method callback function: to save the transaction details made from the shop
 * @param string $order_id
 * @param array $data
 * @param string $type
 * @return none
 */
function uc_novalnet_save_log($order_id, $data, $type) {
    /* Stores the payment request in novalnet logs*/
    if ($type == 'request' && isset($data['key'])) {
        /* Updating the pan hash as masked */
        if (!empty($data['pan_hash']) && !empty($data['unique_id'])) {
            unset($data['unique_id'], $data['pan_hash']);
        }
    }
    /* Stores the payment response in novalnet logs*/
    if ($type == 'response' && !is_array($data)) {
        parse_str($data, $raw_data);
        $data = $raw_data;
    }
    $db_data = array(
        'id' => '',
        'order_id' => $order_id,
        'logtype'  => $type,
        'logvalue' => serialize($data),
        'logtime'  => date('d.m.Y H:i:s ')
    );
    drupal_write_record('uc_novalnet_logs', $db_data);
}

/**
 *
 * Payment method callback function: request to the payment server
 *
 * @param array $request
 * @param string $type (is curl or xml)
 * @param string $order_no
 * @return array | string
 */
function uc_novalnet_send_payport($request, $type = '', $order_no = '') {
    if (array_search('', array_map('trim', $request)) && $type != 'curl') {
        drupal_set_message(t('Required param missing!'), 'error');
        return FALSE;
    }
    /* To save the request log */
    uc_novalnet_save_log((!empty($request['order_no']) ? $request['order_no'] : $order_no), $request, 'request');
    /* Process the Curl request */
    if ($type == 'curl') {
        $xml  = http_build_query($request, '', '&');
        $host = uc_novalnet_get_url('nncurl_request');
    }
    /* Process the xml request */
    else {
        $xml = "<?xml version='1.0' encoding='UTF-8'?>
            <nnxml>
            <info_request>";
        foreach ($request as $tag => $val) {
            $xml .= "<" . $tag . ">" . $val . "</" . $tag . ">";
        }
        $xml .= "</info_request>
            </nnxml>";
        $host = uc_novalnet_get_url('nnxml_request');
    }
    $options = array(
		'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
		'method' => 'POST',
		'data' => $xml,
	);

    $global_setting = variable_get('novalnet_params', '');
    
	if (!empty($global_setting['gateway_timeout'])) {
		$options['timeout'] = $global_setting['gateway_timeout'];
	}

    $response = drupal_http_request($host, $options);
    $result =  (($type == 'curl') ? (isset($response->data) ? $response->data : 'status=408&status_desc=Payment was not successful. An error occurred.') : (isset($response->data) ? json_decode(json_encode(simplexml_load_string($response->data)), TRUE) : 'status=408&status_desc=Payment was not successful. An error occurred.'));
    uc_novalnet_save_log((!empty($request['order_no']) ? $request['order_no'] : $order_no), $result, 'response');
    return $result;
}

/**
 *
 * Payment method callback function: to maintain payment information in session, while using fraud prevention
 *
 * @param object $order
 * @return string
 */
function uc_novalnet_storervalues( $order ) {
    $payment      = $order->payment_method;
    $basic_data = basic_params();
    /* To get enabled fraud prevention method */
    $fraud_method = variable_get($payment . '_pin_method', '');
    if ($fraud_method && isset($_SESSION['novalnet_session'][$payment][$payment . '_pinkey'])) {
        $data = array();
        /* Load the payment call params */
        uc_novalnet_getparams($data, $order);
        /* Check the TID is stored in payment session */
        if (!isset($_SESSION['novalnet_session'][$payment]['tid'])) {
            if (variable_get($payment . '_pin_method')) {
                $fraud_array = array( 'callback'  => array('pin_by_callback', 'tel'),
                                      'sms'       => array('pin_by_sms', 'mobile'));
                $data[$fraud_array[$fraud_method][0]] = 1;
                $data[$fraud_array[$fraud_method][1]] = $_SESSION['novalnet_session'][$payment][$payment . '_pinkey'];
            }
            /* Checks the stored order amount is grater than order total */
            if (variable_get($payment . '_pin_amount', '') && sprintf('%0.2f', $order->order_total) * 100 < variable_get($payment . '_pin_amount')) {
                drupal_set_message(t('The order amount has been changed, please proceed with the new order'), 'error');
                drupal_goto('cart/checkout');
            }
            /* Send the payment params to Novalnet payport */
            $response = uc_novalnet_send_payport($data, 'curl');
            parse_str($response, $parsed);
            /* Store the response from the first call */
            $_SESSION['novalnet_session'][$payment]['masked_acc_details'] = $parsed;
            /* To check the Transaction status form the response */
            $status_message = uc_novalnet_check_status($parsed);
            if ($status_message != 'success') {
                drupal_set_message(check_plain($status_message), 'error');
                watchdog('uc_novalnet', '<br/> Payment Error: @error', array('@error' => $status_message), WATCHDOG_ERROR);
                uc_novalnet_setsession($payment);
            }
            else {
                /* store the payment details in session for first call response during the fraud check*/
                $session_array = array('tid' => $parsed['tid'], 'amount' => $parsed['amount'], 'test_mode' => $parsed['test_mode']);
                if ($payment == 'novalnet_invoice')
                $session_array = array_merge($session_array, array('invoice_bankcode' => $parsed['invoice_bankcode'],
                                                               'invoice_iban'       => $parsed['invoice_iban'],
                                                               'invoice_ref'        => "BNR-" . $basic_data['product_id'] . "-" . $parsed['order_no'],
                                                               'due_date'           => $parsed['due_date'],
                                                               'invoice_bic'        => $parsed['invoice_bic'],
                                                               'invoice_bankname'   => $parsed['invoice_bankname'],
                                                               'invoice_bankplace'  => $parsed['invoice_bankplace']));
                $message_array = array('callback'  => t('You will shortly receive a transaction PIN through phone call to complete the payment'),
                                    'sms'  => t('You will shortly receive an SMS containing your transaction PIN to complete the payment'),
                );
                /* Store the session values in Novalnet session */
                uc_novalnet_setsession($payment, $session_array);
                drupal_set_message(check_plain($message_array[$fraud_method]), 'warning');
            }
            drupal_goto('cart/checkout');
        } else{
            if (!empty($_SESSION['novalnet_session'][$payment]['forget_pin']) && $_SESSION['novalnet_session'][$payment]['forget_pin']) {
                /* Processed with forget pin */
                $parsed = uc_novalnet_send_payport(array(
                              'vendor_id'       => $data['vendor'],
                              'vendor_authcode' => $data['auth_code'],
                              'tid'             => $_SESSION['novalnet_session'][$payment]['tid'],
                              'lang'            => $data['lang'],
                              'request_type'    => 'TRANSMIT_PIN_AGAIN'
                              ), '', $data['order_no']);
                $msg = !empty($parsed['status_message']) ? $parsed['status_message'] : $parsed['status_desc'];
                if ($parsed['status'] == '0529008' ) {
                    unset($_SESSION['novalnet_session'][$order->payment_method]);
                }
                drupal_set_message(check_plain($msg), 'warning');
                drupal_goto('cart/checkout');
            }
        }
    }
    /* To unset the entire payment session */
    if (!isset($_SESSION['novalnet_session'][$payment])) uc_novalnet_setsession();
    else uc_novalnet_setsession($payment, $_SESSION['novalnet_session'][$payment], TRUE);
}

/**
 *
 * Payment method callback function: to store the session details and also to unset them payment wise.
 *
 * @param string $payment
 * @param array $values
 * @param boolean $reset
 * @return none
 */
function uc_novalnet_setsession($payment = '' , $values = array(), $reset = FALSE) {
    /* Unset the entire Novalnet session when the flag is enabled($reset == TRUE) OR Payment is passed as empty */
    if ((isset($_SESSION['novalnet_session']) && empty($payment)) || $reset) unset($_SESSION['novalnet_session']);
    /* Adding the values($values) into payment session array, if the values are not not empty */
    if (!empty($values)) {
        $_SESSION['novalnet_session'][$payment] = isset($_SESSION['novalnet_session'][$payment]) ? array_merge( $_SESSION['novalnet_session'][$payment], $values) : $values;
    }
}

/**
 *
 * Payment method callback function: to update the callback amount
 *
 * @param object $order
 * @param string $messages
 * @param string $due_date
 * @return string
 */
function callback_amt_update($order, $messages, $due_date='') {
    /* $updated_data -  Fetch the due_date from uc_novalnet_transaction table */
    $updated_data = nn_variable_get(array('order_id' => $order->order_id), 'uc_novalnet_transaction');
    /* Fetch the Previous order comments from the receipts to rebill the transaction */
    $result = nn_variable_get(array('order_id' => $order->order_id), 'uc_payment_receipts', array('comment'));
    if (in_array($order->payment_method, array('novalnet_sepa', 'novalnet_cc', 'novalnet_paypal')))
        $result['comment'] = preg_replace('|<span id="tid_update">(.+?)</span>|s', $updated_data['tid'], $result['comment']);

    /* Update the order amount and due date during the extension(amount update / due date update) process */
    if (in_array($order->payment_method, array('novalnet_invoice', 'novalnet_prepayment'))) {
        $due_date = !empty($due_date) ? date('m/d/Y', strtotime($due_date)) : date('m/d/Y', strtotime($updated_data['due_date']));
        $result['comment'] = preg_replace("/\d{2}\/\d{2}\/\d{4}/", $due_date , $result['comment']);
        update_variable(array('order_id' => $order->order_id), array('comment' =>  $result['comment']), 'uc_payment_receipts');
        /* To update the due date in novalnet transaction table */
        update_variable(array('order_id' => $order->order_id), array('due_date' =>  date('Y-m-d H:i:s', strtotime($due_date))), 'uc_novalnet_transaction');
        if (!empty($_SESSION['novalnet_session'][$order->payment_method]['updated_amount']))
        $result['comment'] = preg_replace('|<span id="amount_update">(.+?)</span>|s', '<span id="amount_update">' . t('Amount : @amount', array('@amount' => uc_currency_format($_SESSION['novalnet_session'][$order->payment_method]['updated_amount']))) . '</span>' , $result['comment']);
    }
    update_variable(array('order_id' => $order->order_id), array('comment' =>  $result['comment']), 'uc_payment_receipts');
    $value = $result['comment'] . "<br/>";
    return $messages;
}

/**
 *
 * Payment method callback function: to update the columns in payment table
 *
 * @param array $condition
 * @param array $data
 * @param string $table
 * @return none
 */
function update_variable($condition, $data, $table) {
    /* If counter value is less than 3 Update the values($data - array) in table($table = table_name) using the condition($condition = condition) */
    if (count($data) <= 3)
        db_update($table)->fields($data)->condition(key($condition), current($condition))->execute();
    /* else block - write the records into the table */
    else {
        if ($table == 'uc_novalnet_transaction') {
            $data['order_id']   = current($condition);
            $data['paymentkey'] = $data['key']; unset($data['key']);
        }
        db_insert($table)->fields($data)->execute();
    }
}

/**
 *
 * Payment method callback function: to fetch the columns from Tables
 *
 * $id           - Primary condition
 *
 * $table        - Table to select
 *
 * $column       - Column to be fetched from the table
 *
 * $condition    - Based on the condition the values to be fetched
 *
 * $condition2   - Based on the condition the values to be fetched
 *
 * @param array $id
 * @param string $table
 * @param string $column
 * @param string $condition
 * @param string $condition2
 * @return array
 */
function nn_variable_get($id, $table, $column = array(), $condition = '', $condition2 = '') {
    /* If the column passed as empty - it will fetch all the columns based on the condition($id) */
    if (empty($column))
        return db_select($table, 'nn')
                ->fields('nn')
                ->condition(key($id), current($id))
                ->execute()
                ->fetchAssoc();
    elseif (!empty($condition) && empty($condition2))
        return db_select($table, 'nn')
                ->fields('nn', $column)
                ->condition(key($id), current($id))
                ->condition(key($condition), current($condition))
                ->orderBy('order_id', 'DESC')
                ->execute()
                ->fetchAssoc();
    elseif (!empty($condition2))
        return db_select($table, 'nn')
                ->fields('nn', $column)
                ->condition(key($id), current($id))
                ->condition(key($condition), current($condition))
                ->condition(key($condition2), current($condition2))
                ->orderBy('order_id', 'DESC')
                ->execute()
                ->fetchAssoc();
    elseif ($table == 'uc_novalnet_cancel' && in_array('ref_orders', $column))
        return db_select($table, 'nn')
                ->fields('nn', $column)
                ->condition(key($id), current($id))
                ->execute()
                ->fetchcol();
    else
        return db_select($table, 'nn')
                ->fields('nn', $column)
                ->condition(key($id), current($id))
                ->orderBy('order_id', 'DESC')
                ->execute()
                ->fetchAssoc();
}

/**
 *
 * Implements hook_init().
 *
 * @params none
 * @return none
 */
function uc_novalnet_init() {
	
	if (!empty($_POST['tid']) && empty($_SESSION['nn']) && empty($_POST['sess_lost'])
	&& !preg_match("/callback_novalnet2ubercart/i", current_path())) {
		$_POST['sess_lost'] = 1;
		$return_url = url(current_path(), array('absolute' => TRUE));
		$params = drupal_http_build_query($_POST);
		drupal_goto($return_url.'?'.$params);
	}

    /* Checking th drupal front page for affiliate process */
    if (drupal_is_front_page()) {
        $request = $_REQUEST;
        if (!empty($request['nn_aff_id'])) {
            $_SESSION['nn']['nn_aff_id']  = $request['nn_aff_id'];
            $_SESSION['nn']['update']     = TRUE;
        }
        else {
            global $user;
            /* Get the current logged user details from Novalnet affiliate (nn_aff_user_detail) table */
            $aff_details = nn_variable_get(array('customer_id' => $user->uid), 'nn_aff_user_detail');
            if (!empty($aff_details['aff_id'])) {
                $_SESSION['nn']['nn_aff_id'] = $aff_details['aff_id'];
                $_SESSION['nn']['update']     = TRUE;
            }
        }
    }
}

/**
 *
 * Payment method callback function : to fetch the basic params
 *
 * @params none
 * @return array
 */
function basic_params($stored_tid = '') {
    /* Get the transaction details using TID */
    if ($stored_tid) {
        return nn_variable_get(array('tid' => $stored_tid), 'uc_novalnet_transaction');
    }
    else {
        /* Get the Novalnet params from back-end */
        $basic_params = variable_get('novalnet_params', '');
        /* Get the affiliate details from the affiliate (nn_aff_account_detail) table if affiliate id present */
        if (!empty($_SESSION['nn']['nn_aff_id'])) {
            $aff_details = nn_variable_get(array('aff_id' => $_SESSION['nn']['nn_aff_id']), 'nn_aff_account_detail');
            $basic_params['vendor_id']          = $aff_details['aff_id'];
            $basic_params['auth_code']          = $aff_details['aff_authcode'];
            $basic_params['payment_access_key'] = $aff_details['aff_accesskey'];
        }
    }
    return $basic_params;
}

/**
 *
 * Payment method validate function: to validate the Direct Debit SEPA duedate
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_duedate($element, &$form_state) {
    $payment = $form_state['build_info']['args'][0];
    $element_value = trim($element['#value']);
    if ($payment != 'novalnet_sepa') return TRUE;

    if ($element_value != '') {
        if (!preg_match("/^[0-9]+$/", $element_value) || $element_value < 2 || $element_value > 14) {
            $errormsg = t('SEPA Due date is not valid');
            form_error($element, check_plain($errormsg));
        }
    }
}

/**
 *
 * Payment method callback function: to store the session values for Direct Debit SEPA
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_nnpaypalform_validate($element, &$form_state) {
    /* if form bulids from the shop front-end, got different form state values */
    if (!empty($form_state['values']['panes']['payment']['details'])) {
        $form_values = $form_state['values']['panes']['payment']['details'];
        $payment = $form_state['values']['panes']['payment']['payment_method'];
    }/* Form bulids from the admin reorder */
    else {
        $form_values = $form_state['input'];
        $payment = $form_values['payment_method'];
    }
    if ($payment != 'novalnet_paypal') return TRUE;
        uc_novalnet_setsession($payment, array(
            'paypal_given_details'    => $form_values['paypal_given_details'],
        ));
    if ((!empty($form_values['one_shop_paypal_enabled']) && $form_values['one_shop_paypal_enabled'] == 1) && variable_get($payment . '_enable_one_shop_clicking', 0) == 1 && (!empty($form_values['paypal_given_details']) && $form_values['paypal_given_details'] == 1))  {

        uc_novalnet_setsession($payment, array(
        'ref'                     => 1,
        'one_shop_paypal_enabled' => $form_values['one_shop_paypal_enabled'],
        ));
        return TRUE;
    }
}

/**
 *
 * Payment method callback function: to store the form values in session for Credit Card
 *
 * $payment - To get the current payment name
 *
 * $form_values - array that contains the form values for the current payment
 *
 * @param  array $element
 * @param  array $form_state
 * @return mixed
 */
function uc_novalnet_nnccform_validation($element, &$form_state) {
  /* if form bulids from the shop front-end, got different form state values */
  if (!empty($form_state['values']['panes']['payment']['details'])) {
      $form_values = $form_state['values']['panes']['payment']['details'];
      $payment = $form_state['values']['panes']['payment']['payment_method'];
  }
   /* Form bulids from the admin reorder */
  else {
    $form_values = $form_state['input'];
    $payment = $form_values['payment_method'];
  }
  if ($payment != 'novalnet_cc') return TRUE;
      if ($form_values['nn_cc_hash'] != '') {
          uc_novalnet_setsession($payment, array(
             'nn_cc_hash'     => trim($form_values['nn_cc_hash']),
             'novalnet_cc_uniqueid'    => trim($form_values[$payment . '_uniqueid']),
          ));
      }
  return TRUE;
}

/**
 *
 * Payment method callback function: to store the form values in session for Credit Card
 *
 * $payment - To get the current payment name
 *
 * $form_values - array that contains the form values for the current payment
 *
 * @param  array $element
 * @param  array $form_state
 * @return mixed
 */
function uc_novalnet_nnccform_validate($element, &$form_state) {
    /* if form bulids from the shop front-end, got different form state values */
    if (!empty($form_state['values']['panes']['payment']['details'])) {
        $form_values = $form_state['values']['panes']['payment']['details'];
        $payment = $form_state['values']['panes']['payment']['payment_method'];
    }/* Form bulids from the admin reorder */
    else {
        $form_values = $form_state['input'];
        $payment = $form_values['payment_method'];
    }
    if ($payment != 'novalnet_cc') return TRUE;
    
    
    uc_novalnet_setsession($payment, array(
        'one_shop_cc_enabled' => !empty($form_values['one_shop_cc_enabled']) ? $form_values['one_shop_cc_enabled'] : '',
        ));
        
    /* Store the form value and set the payment ref flag as TRUE, if one step shopping enabled */
    if ((!empty($form_values['one_shop_cc_enabled']) && $form_values['one_shop_cc_enabled'] == 1) && variable_get($payment . '_enable_one_shop_clicking', 0) == 1 && (!empty($form_values['one_page_shopping']) && $form_values['one_page_shopping'] == 1) && (empty($form_values['nn_cc_hash']) && empty($form_values['novalnet_cc_uniqueid'])))  {
        uc_novalnet_setsession($payment, array('ref'  => 1, 'form_type' => 1));
        return TRUE;
    }
}

/**
 *
 * Payment method callback function: to store the session values for Direct Debit SEPA
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_nnsepaform_validate($element, &$form_state) {

    /* if form bulids from the shop front-end, got differnt form values */
    if (!empty($form_state['values']['panes']['payment']['details'])) {
        $form_values = $form_state['values']['panes']['payment']['details'];
        $payment = $form_state['values']['panes']['payment']['payment_method'];
    } /* Form bulids from the admin reorder */
    else {
        $form_values = $form_state['input'];
        $payment = $form_values['payment_method'];
    }

    if ($payment != 'novalnet_sepa') return TRUE;
    /* Set the flag for reference order, if the payment is done through previous account details*/
    if ((!empty($form_values['one_shop_sepa_enabled']) && $form_values['one_shop_sepa_enabled'] == 1)  && empty($form_values['sepa_given_details'])) {
        uc_novalnet_setsession($payment, array(
        'ref'           => 1,
        'form_type'     => 0
        ));
        return TRUE;
    }
    /* organise the session values for sepa payment - with local form */
    if (!empty($form_values['nnsepa_iban']) ) {
        uc_novalnet_setsession($payment, array(
          'nnsepa_owner'    => trim($form_values['nnsepa_owner']),
          'nnsepa_iban'  => trim($form_values['nnsepa_iban']),
          'form_type'       => 1
        ));
    }
}

/**
 *
 * Back end validation for payment references:
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function payment_gurantee_validation($element, &$form_state) {
    $gurantee_validate = $form_state['values'];
    $payment = $form_state['build_info']['args'][0];

/* validate the payment reference - atleast one reference should enabled in the shop back-end */
    if (!empty($gurantee_validate[$payment . '_payment_guarantee_check'])) {
        if (!empty($gurantee_validate[$payment . '_minimum_payment_guarantee_check']) && $gurantee_validate[$payment . '_minimum_payment_guarantee_check'] < '999') {
            form_error($element, check_plain(t('The minimum amount should be at least 9,99 EUR')));
            return ;
        }
    }
}

/**
 *
 * Payment method validate function: to validate the email address
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_email($element, &$form_state) {
    $emails = explode(',', $element['#value']);
    foreach ($emails as $email) {
        if (($form_state['values']['cbk_mail_send'] == 0) && !valid_email_address($email)) {
            $errormsg = $element['#title'] . ' ' . t('is invalid');
            form_error($element, check_plain($errormsg));
        }
    }
}

/**
 *
 * Payment method validate function: for birthdate
 *
 * @param  array $element
 * @param  array $form_state
 * @return none
 */
function uc_novalnet_element_validate_birth_date($element, &$form_state) {
    /* if form bulids from the shop front-end, got differnt form values */
    if (!empty($form_state['values']['panes']['payment']['details'])) {		
        $form_values = $form_state['values']['panes']['payment']['details'];
        $payment = $form_state['values']['panes']['payment']['payment_method'];
        $dob = $element['#value']['year'] . '-' . $element['#value']['month'] . '-' . $element['#value']['day'];
        $birth_date = $dob;
    } /* Form bulids from the admin reorder */
    else {
        $form_values = $form_state['input'];
        $payment = $form_values['payment_method'];
        $dob = $form_values['birth_date']['year'] . '-' . $form_values['birth_date']['month'] . '-' . $form_values['birth_date']['day'];
        $birth_date = $dob;
    }
    if (!empty($dob)) {
        /* To get the age details from the user input */
        $dob = (time() - strtotime($dob))/31536000;
        uc_novalnet_setsession($payment, array($payment . '_birth_date'  => $birth_date));
       
    }
}

/**
 *
 * Function to add birth date and payment keys for guaranteed payments.
 *
 * @param array data
 * @param string payment
 * @return none
 */
function uc_novalnet_check_guarantee_payments(&$data, $order) {
    $gurantee_implements = '';
    $billing_country = uc_get_country_data(array('country_id' => $order->billing_country));
    $delivery_address = array($order->delivery_street1, $order->delivery_street2, $order->delivery_city, $order->delivery_postal_code, $order->delivery_country);
    $billing_address = array($order->billing_street1, $order->billing_street2, $order->billing_city, $order->billing_postal_code, $order->billing_country);
    $birth_date = isset($_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_birth_date']) ?  $_SESSION['novalnet_session'][$order->payment_method][$order->payment_method . '_birth_date'] : '';
    $min_amount = variable_get($order->payment_method . '_minimum_payment_guarantee_check') ? variable_get($order->payment_method . '_minimum_payment_guarantee_check') : 999;
	$order_amt = $order->order_total * 100;
	$dob = (time() - strtotime($birth_date))/31536000;
	
	echo $birth_date.'/'.$dob;
    if (in_array($billing_country[0]['country_iso_code_2'], array('AT', 'DE', 'CH')) && (($order_amt >= $min_amount) && $order->currency == 'EUR') && ($delivery_address === $billing_address) && ($dob > 18 || !empty($order->billing_company))) {
	
        /* Get the payment key for the guaranteed payment */
        $key = uc_novalnet_payment_methods($order->payment_method);
        $data['key']         = $key['guarantee_key'];
        if(!empty($order->billing_company)) $data['company']  = $order->billing_company;
        else $data['birth_date']  = $birth_date;
        $gurantee_implements = 'gurantee';
	}
    $_SESSION['novalnet_session'][$order->payment_method]['gurantee'] = $gurantee_implements;
}

/**
 *
 * Function to get original transaction amount
 *
 * @param array tid_details
 * @param integer amount
 * @param boolean $get_amount
 * @return array
 */
function uc_novalnet_get_original_amount($tid_details = array(''), $amount = '', $get_amount = FALSE) {
    $original_amount  = unserialize($tid_details['masked_acc_details']);
    if ($get_amount) {
        return empty($original_amount['original_amount']) ? $tid_details['callback_total'] : $original_amount['original_amount'] ;
    }
    $original_amount['original_amount'] = $amount;
    return serialize($original_amount);
}

/**
 *
 * Function to hide the novalnet payments in shop front-end (used for pin exceeds and guarantee payments)
 *
 * @param array $payment_array (referenced)
 * @return boolean
 */
function uc_novalnet_unset_payment(&$payment_array) {
    if (isset($_SESSION['novalnet_error_payment']) && arg(0) != 'admin') {
        foreach (array_keys($_SESSION['novalnet_error_payment']) as $key) {
            if (isset($_SESSION['novalnet_error_payment'][$key]['error_time']) && $_SESSION['novalnet_error_payment'][$key]['error_time'] + 30*60 > time())
                unset($payment_array[$key]);
            elseif (!empty($_SESSION['novalnet_error_payment'][$key]['guarantee_check']))
                unset($payment_array[$key]);
            else {
                unset($_SESSION['novalnet_error_payment'][$key]);
            }
        }
    }
}
