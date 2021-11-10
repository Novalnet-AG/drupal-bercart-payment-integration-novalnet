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
 * Script   : uc_novalnet_admin.inc
 *
 * This file contains the Conditional Actions, hooks and functions necessary to make
 * admin related actions in the shop systems(E.g: Extension features provide by Novalnet).
 */

    module_load_include('module', 'uc_novalnet', '/includes/uc_novalnet');
    module_load_include('inc', 'uc_novalnet', '/includes/uc_novalnet');

/**
 *
 * Implements hook_menu().
 *
 * @params none
 * @return array $items
 */
function uc_novalnet_menu() {
    $items = array();
    /* Menu callback(return_url) for redirect payments to complete the order */
    $items['novalnet_order/review'] = array(
        'title'            => 'Review payment',
        'page callback'    => 'uc_novalnet_nnpayment_complete',
        'access arguments' => array('access content'),
        'type'             => MENU_CALLBACK,
        'file'             => 'includes/uc_novalnet.inc',
    );
    /* Menu callback for callback script */
    $items['callback_novalnet2ubercart'] = array(
        'title'            => 'callbackscript',
        'page callback'    => 'uc_novalnet_callbackscript',
        'access arguments' => array('access content'),
        'type'             => MENU_CALLBACK,
        'file'             => 'includes/uc_novalnet_page.inc',
    );
    /* Novalnet menu creation for configuring the global configuration and fetch the Novalnet orders */
    $items['admin/novalnet'] = array(
        'title'            => 'Novalnet',
        'menu_name'        => 'management',
        'description'      => 'Novalnet' . ' ' . t('Configuration'),
        'page callback'    => 'system_admin_menu_block_page',
        'access arguments' => array('administer site configuration'),
        'file'             => 'system.admin.inc',
        'file path'        => drupal_get_path('module', 'system'),
    );
    /* Menu callback Novalnet global configuration ( Novalnet -> Novalnet Global Configuration ) */
    $items['admin/novalnet/globalconfig'] = array(
        'title callback'   => 'uc_novalnet_global_config_title',
        'title arguments'  => array('Novalnet Global Configuration'),
        'weight'           => 2,
        'page callback'    => 'drupal_get_form',
        'page arguments'   => array('uc_novalnet_global_config_form'),
        'access arguments' => array('administer site configuration'),
        'file'             => 'uc_novalnet.module'
    );
    /* Menu callback for vendor details refil ( Novalnet -> Novalnet Global Configuration ) */
    $items['admin/novalnet/autofill_paymentdata'] = array(
        'title' => t('payment_data_config'),
		'file'             => 'includes/uc_novalnet_form.inc',
		'type' => MENU_CALLBACK,
		'page callback' => 'uc_novalnet_publickey_submit',
		'access arguments' => array('access content'),
	);
    /* Menu callback for Novalnet orders ( Novalnet -> Novalnet orders ) */
    $items['admin/novalnet/novalnet_orders'] = array(
        'title'            => 'Novalnet' . ' ' . t('Orders'),
        'page callback'    => 'drupal_goto',
        'access arguments' => array('Extension'),
        'page arguments'   => array('admin/store/novalnet_orders/view'),
        'weight'           => -99
    );
    /* Menu callback for log the Novalnet transaction details @ request and @ responses  */
    $items['admin/novalnet/novalnet_orders/%uc_order/novalnet_log'] = array(
        'title'            => 'Novalnet Transaction Log',
        'page callback'    => 'uc_novalnet_payment_logs',
        'page arguments'   => array(3),
        'access arguments' => array('view all orders'),
        'type'             => MENU_LOCAL_TASK,
        'weight'           => 10,
        'file'             => 'includes/uc_novalnet_form.inc',
    );
    /* Menu callback for Paypal configuartion link in Novalnet global configuration */
    $items['merchant_paypal_config'] = array(
        'title'            => '',
        'type'             => MENU_NORMAL_ITEM,
        'page callback'    => 'drupal_get_form',
        'page arguments'   => array('uc_novalnet_merchant_admin'),
        'access arguments' => array('administer site configuration')
    );
    /* Menu callback to fetch the individual orders from Novalnet orders */
    $items['admin/store/orders/%uc_order/novalnet_payment'] = array(
    'title callback' => 'ttile',
    'title arguments' => array(3),
    'description' => 'Displays a form to process a Novalnet payment.',
    'page callback' => 'uc_novalnet_admin_reorder',
    'page arguments' => array(3),
    'access arguments' => array('process payment'),
    );    
      
  
    return $items;
}

/**
 *
 * Implementation of callback title
 *
 * @params string $title
 * @return string
 */
function uc_novalnet_global_config_title($title) {
  return t($title);
}

/**
 * Implements hook_views_api().
 *
 * @params none
 * @return array
 */
function uc_novalnet_views_api() {
    return array(
        'api'  => 3,
        'path' => drupal_get_path('module', 'uc_novalnet') . '/views',
    );
}

/**
 *
 * Implements hook_form_alter().
 *
 * (Core function - Perform alterations before a form is rendered)
 *
 * @params array $form
 * @params array $form_state
 * @params string $form_id
 * @return none
 */
function uc_novalnet_form_alter(&$form, &$form_state, $form_id) {
    /* Novalnet payment settings handling */
    if ($form_id == 'uc_payment_methods_form') {
        /* Get the Basic parameters */
        $config_data = basic_params();
        /* Setting up the flag for displaying the intimation to admin to configure the payment fields properly */
        $error_preserved = FALSE;
        if (empty($config_data)) {
            /* Get the Novalnet payment methods */
            foreach (uc_novalnet_payment_methods() as $key => $val) {
                $form['pmtable'][$key]['#disabled'] = TRUE;
            }
            $error_preserved = TRUE;
        }
        if ($error_preserved) drupal_set_message(t('Please configure') . ' ' . l(t('Novalnet Global Configuration') . ' ' , 'admin/novalnet/globalconfig') . t('to enable the Novalnet Payment method'), 'warning');
    }

    /* Alters the order form for extension process */
    if ($form_id == 'uc_order_view_update_form') {
        $form['#attributes']['autocomplete'] = 'off';
        $order        = uc_order_load($form['order_id']['#value']);
        /* Getting the order details from Novalnet table */
        $tid_details  = nn_variable_get(array('order_id' => $order->order_id), 'uc_novalnet_transaction');
        /* Zero amount booking for the possible payments and check the zero amout flag is enabled for the current payment */
        $zero_payments_is_active = ((in_array($order->payment_method, array('novalnet_cc', 'novalnet_sepa', 'novalnet_paypal')) && $tid_details['amount'] == 0 ) ? 1 : 0);
        /* Payment array - allowed payments for extension process */
        $allowed_for_extension = array('6', '37', '34', '49', '33', '50', '69', '40', '78', '41','59');
        /* checking the transaction status for extension process */
        if ((!empty($tid_details['status']) && in_array($tid_details['status'], array(85, 90, 91, 99, 98, 100)))) {
            /* Checking the allowed payments with the suitable status for void and capture processes and payment allowed for void-capture process */
            if (in_array($tid_details['status'], array(85, 91, 99, 98)) && in_array($tid_details['paymentkey'], array('27', '6', '37', '34', '40', '41'))) {
                $form['nn_void_capture'] = array(
                    '#type'        => 'fieldset',
                    '#title'       => t('Manage transaction process'),
                    '#collapsible' => 1,
                    '#collapsed'   => 1,
                    '#attributes'  => array('id' => 'void_capture_fieldset')
                );
                $form['nn_void_capture']['on_hold_transaction_enable'] = array(
                   '#type'        => 'checkbox',
                   '#title'       => t('Manage transaction process'),
                   '#attributes'  => array('id' => 'transaction_enable', 'data-text' => 'amount_update_fieldset')
                );
                $form['nn_void_capture']['on_hold_transaction'] = array(
                    '#type'          => 'select',
                    '#options'       => array('' => t('--Select--'), 'capture' => t('Confirm'), 'void' => t('Cancel')),
                    '#default_value' => t('--select--'),
                    '#states'        => array('visible' => array('input[id="transaction_enable"]' => array('checked'      => TRUE))),
                );
            }
            /* Checking the allowed payemnts with the suitable status for amount update processes */
            if ((($tid_details['paymentkey'] == 27 && $tid_details['status'] == '100') || ($tid_details['paymentkey'] == '37' && $tid_details['status'] == '99')) && (int) $tid_details['amount'] > 0 || $tid_details['paymentkey'] == 59) {
                if ($tid_details['paymentkey'] == '27') {
                  $title = t('Change the amount / due date');
                 /* If it is invoice prepayment we have to show the total order amount */
                  $amount = uc_novalnet_get_original_amount($tid_details, '', TRUE);
                }
                else if ($tid_details['paymentkey'] == '37') {
                    $title = t('Amount update');
                    $amount = '';
                } else {
                      $title = t('Change the amount/slip expiry date');
                     /* If it is invoice prepayment we have to show the total order amount */
                      $amount = uc_novalnet_get_original_amount($tid_details, '', TRUE);
                }
                if (empty($amount))
                  $amount = $tid_details['callback_total'];

                $form['nn_amount_update'] = array(
                    '#type'        => 'fieldset',
                    '#title'       => check_plain($title),
                    '#collapsible' => 1,
                    '#collapsed'   => 1,
                    '#attributes'  => array('id' => 'amount_update_fieldset')
                );
                $form['nn_amount_update']['nn_amount_update_enable']  = array(
                    '#type'        => 'checkbox',
                    '#title'       => check_plain($title),
                    '#attributes'  => array('id' => 'enable_amount_update', 'data-text' => 'void_capture_fieldset', 'data-refund' => 'refund_fieldset')
                );
                $form['nn_amount_update']['transaction_amount_change']  = array(
                    '#type'        => 'textfield',
                    '#title'       => t('Update transaction amount') . ' ' . t('(in minimum unit of currency. E.g. enter 100 which is equal to 1.00)') ,
                    '#attributes'  => array('onkeypress' => 'return allow_numbers_only(event)'),
                    '#description' =>  strip_tags(t('Novalnet transaction ID%s', array('%s' => ': ' . $tid_details['tid']))),
                    '#states'      => array('visible' => array('input[id="enable_amount_update"]' => array('checked'      => TRUE))),
                    '#default_value' => $amount
                );
                if ($tid_details['paymentkey'] == '59')
                $form['nn_amount_update']['cashpayment_duedate_change']  = array(
                    '#type'   => 'textfield',
                    '#title'  => t('Change the slip expiry date'),
                    '#default_value' => isset($tid_details['due_date']) ? date('m/d/Y', strtotime($tid_details['due_date'])) : '',
                    '#states' => array('visible' => array('input[id="enable_amount_update"]' => array('checked' => TRUE))),
                );
                if ($tid_details['paymentkey'] == '27')
                $form['nn_amount_update']['invoice_duedate_change']  = array(
                    '#type'   => 'textfield',
                    '#title'  => t('Transaction due date'),
                    '#default_value' => isset($tid_details['due_date']) ? date('m/d/Y', strtotime($tid_details['due_date'])) : '',
                    '#states' => array('visible' => array('input[id="enable_amount_update"]' => array('checked' => TRUE))),
                );
            }
            /* unset the Credit card and SEPA payment, if it is processed as zero amount */
            if ($tid_details['amount'] == 0)
            unset($allowed_for_extension[0], $allowed_for_extension[1], $allowed_for_extension[2]);
            /* Checking the allowed payments with the suitable status for refund processes */

            if ($tid_details['status'] == 100 && (in_array($tid_details['paymentkey'], $allowed_for_extension) || $tid_details['paymentkey'] == '27')) {
                $form['nn_refund'] = array(
                    '#type'        => 'fieldset',
                    '#title'       => t('Refund'),
                    '#collapsible' => 1,
                    '#collapsed'   => 1,
                    '#attributes'  => array('id' => 'refund_fieldset')
                );
                $form['nn_refund']['nn_refund_enable']  = array(
                    '#type'        => 'checkbox',
                    '#title'       => t('Refund process'),
                    '#attributes'  => array('id' => 'enable_nn_refund', 'data-text' => 'amount_update_fieldset')
                );
                if ($tid_details['paymentkey'] == '27' && empty($tid_details['amount']))
                $tid_details['callback_total'] = $tid_details['callback_amount'];

                $form['nn_refund']['transaction_refund']  = array(
                   '#type'          => 'textfield',
                   '#title'         => t('Please enter the refund amount') . ' ' . t('(in minimum unit of currency. E.g. enter 100 which is equal to 1.00)'),
                   '#description'   =>  strip_tags(t('Novalnet transaction ID%s', array('%s' => ': ' . $tid_details['tid']))),
                   '#attributes'    => array('onkeypress' => 'return ccnumber_validate(event)'),
                   '#default_value' => uc_novalnet_get_original_amount($tid_details, '', TRUE),
                   '#states'        => array('visible' => array('input[id="enable_nn_refund"]' => array('checked'      => TRUE))),
                );
                    $refund_ref_visible =  (time() >= strtotime("+1 days", $order->created)) ? TRUE : FALSE ;
                    /* Set the Refund reference flag after one day of order creation */
                if ($refund_ref_visible) {
                    $form['nn_refund']['refund_ref']  = array(
                       '#type'   => 'textfield',
                       '#title'  => t('Refund reference'),
                    );
                }
            }
        }
        /* Checking the allowed payments with the suitable status for amount booking processes */
        if (in_array($tid_details['paymentkey'], array('6', '37', '34')) && in_array($tid_details['status'], array(90, 99, 98, 100)) && $zero_payments_is_active) {
            $form['nn_book'] = array(
                '#type'    => 'fieldset',
                '#title'  => t('Book transaction'),
                '#collapsible' => 1,
                '#collapsed'   => 1,
            );
            $form['nn_book']['nn_book_enable']  = array(
                '#type'        => 'checkbox',
                '#title'       => t('Book'),
                '#attributes'  => array('id' => 'nn_book_enable', 'data-text' => 'amount_update_fieldset')
            );
            $form['nn_book']['nn_amount_book_textfield'] = array(
                '#type'            => 'textfield',
                '#title'           => t('Transaction booking amount'),
                '#description'     =>  strip_tags(t('Novalnet transaction ID%s', array('%s' => ': ' . $tid_details['tid']))),
                '#attributes'      => array('id' => 'nn_amount_book_textfield'),
                '#default_value'   => !empty($tid_details['amount']) ? $tid_details['amount'] : round($order->order_total, 2) * 100
            );
        }
        /* form submit handler */
        $form['#submit'][] = 'uc_refund_process_submit';
    }
}

/**
 *
 * Payment method callback function: submit handler
 *
 * @params array $form
 * @params array $form_state
 * @return none
 */
function uc_refund_process_submit($form, &$form_state) {
    $form_values  = array_map('trim', $form_state['values']);
    /* Get the order id from the form state value ( $order - object)*/
    $order        = uc_order_load($form['order_id']['#value']);
    /* Get the Novalnet params from shop back-end */
    $basic_config = basic_params();
    $comments = '';
    /* Get the transaction details from the novalnet transaction table */
    $tid_details  = nn_variable_get(array('order_id' => $order->order_id), 'uc_novalnet_transaction');
    /* Parameter formation for extension process */
    $transaction_call = array(
        'vendor'          => $tid_details['vendor_id'],
        'auth_code'       => $tid_details['auth_code'],
        'product'         => $tid_details['product_id'],
        'tariff'          => $tid_details['tariff_id'],
        'key'             => $tid_details['paymentkey'],
        'tid'             => $tid_details['tid'],
    );
    /* Void & Capture Block ( Line no 360 to 378 )- Organise the params to performing confirmation and cancelation of transaction */
    if (!empty($form_values['on_hold_transaction_enable'])) {
        if ($form_values['on_hold_transaction'] == '') {
            form_set_error('on_hold_transaction', t('Please select status'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        /* Setting up a flag for capturing the transaction($ext_process_type = extension request type ) */
		$order_status = (variable_get($form_state['build_info']['args'][0]->payment_method . '_before_status'));
		
		if ($tid_details['status'] == '85' || $transaction_call['key'] == '41'|| $transaction_call['key'] == '6' || $transaction_call['key'] == '33'|| $transaction_call['key'] == '37') {
			$order_status = (variable_get($form_state['build_info']['args'][0]->payment_method . '_after_status'));
		}
		
        $transaction_call['edit_status'] = 1;
        $transaction_call['status']      = 100;
        $ext_process_type = '';
        /* Setting up a flag for cancelling the transaction */
        if ($form_values['on_hold_transaction'] == 'void') {
            $transaction_call['status'] = 103;
            $ext_process_type = 'onhold_void_cancel_status';
            /* Get the on-hold order status from the novalnet global configuration */
			$order_status    = $basic_config[$ext_process_type];
        }
        /* $update_variable - contains the array values which is to be updated in Novalnet transaction table */
        $update_variable = array('status' => $transaction_call['status']);
        $comments        = ($ext_process_type == 'onhold_void_cancel_status') ? t('The transaction has been canceled on @date, @time', array('@date' => date('Y:m:d'), '@time' => date('h:i:s'))) : t('The transaction has been confirmed on @date, @time', array('@date' => date('Y:m:d'), '@time' => date('h:i:s')));
    }
    /* Amount update block ( Line no 381 to 405 )- Organise the params to perform amount update process */
    elseif (!empty($form_values['nn_amount_update_enable'])) {
        /* Amount validation from the form fields (amount should not be 0) */
        if ($form_values['transaction_amount_change'] == 0) {
            form_set_error('transaction_amount_change', t('The amount is invalid'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        /* Set the amount update flag */
        $ext_process_type = 'amount_update';
        $transaction_call['status']                = 100;
        $transaction_call['amount']                = $form_values['transaction_amount_change']; ### values from the form input ###
        $transaction_call['update_inv_amount']     = 1;
        $transaction_call['edit_status']           = 1;
        /* Validation for due date */
        if ($tid_details['paymentkey'] == 27 && isset($form_values['invoice_duedate_change'])) {
            if ($form_values['invoice_duedate_change'] != date('m/d/Y', strtotime($form_values['invoice_duedate_change']))) {
                form_set_error('invoice_duedate_change', t('Please enter valid due date'));
                drupal_goto('admin/store/orders/' . $order->order_id);
            }
        }
        if ($tid_details['paymentkey'] == 27 && isset($form_values['invoice_duedate_change'])) {
            $transaction_call['due_date'] = date('Y-m-d', strtotime($form_values['invoice_duedate_change']));
            $update_variable = array('amount' => $form_values['transaction_amount_change'], 'due_date' => date('m/d/Y', strtotime($transaction_call['due_date'])));
        } else {
          $update_variable = array('amount' => $form_values['transaction_amount_change']);
          if($tid_details['paymentkey'] == 59)
          $transaction_call['due_date'] = date('Y-m-d', strtotime($form_values['cashpayment_duedate_change']));
          
        }
        $due_date = isset($transaction_call['due_date']) ? $transaction_call['due_date'] : date('m/d/Y', strtotime($tid_details['due_date']));
        $comments        = ($transaction_call['key'] == 37) ? t('The transaction amount @amount has been updated successfully on @date,@time', array('@amount' => uc_currency_format($update_variable['amount'] /100), '@date' => (date('d-m-Y')), '@time' => date('h:i:s') )) : t('The transaction has been updated with amount @amount and due date with @date,@time', array('@amount' => uc_currency_format($update_variable['amount'] /100), '@date' => ($due_date), '@time' => date('h:i:s')));
    }
    /* Refund block ( Line no 406 to 447 )- Organise the params to perform refund process */
    elseif (!empty($form_values['nn_refund_enable'])) {
        /* Amount validation from the form fields (amount should not be 0) */
        if ($form_values['transaction_refund'] == '0') {
            form_set_error('transaction_refund', t('The amount is invalid'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        /* Account details validation from the user input (Iban should not be empty) */
        elseif (!empty($form_values['enable_ddsepa_refund']) && empty($form_values['sepa_iban'])) {
            form_set_error('transaction_refund', t('Your account details are invalid'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        if ($form_values['transaction_refund'] == '') {
            form_set_error('transaction_refund', t('The amount is invalid'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        /* Set the refund flag */
        $ext_process_type = 'refund';
        $transaction_call['status'] = 100;
        $transaction_call['refund_request'] = 1;
        $transaction_call['refund_param']   = $form_values['transaction_refund'];

        /* Pass the refund reference when its not empty */
        if (!empty($form_values['refund_ref']))
        $transaction_call['refund_ref']   = $form_values['refund_ref'];
        /* Checking the flag - Refund with SEPA account details is activated */
        if (isset($form_values['enable_ddsepa_refund']) && $form_values['enable_ddsepa_refund']) {
            $transaction_call['account_holder'] = $form_values['account_holder'];
            $transaction_call['account_no']     = $form_values['sepa_iban'];
        }
        if (in_array($order->payment_method, array('novalnet_invoice', 'novalnet_prepayment'))) {
            $cancel_status = empty($tid_details['amount']) ? TRUE : FALSE;
            $update_variable = array('callback_amount' => $form_values['transaction_refund']);
        }
        else {
            $update_variable = array('amount' => $form_values['transaction_refund']);
            $cancel_status = empty($form_values['transaction_refund']) ? TRUE : FALSE;
        }
        if ($cancel_status) {
            $update_variable = (in_array($order->payment_method, array('novalnet_invoice', 'novalnet_prepayment'))) ? array('status' => 103, 'callback_amount' => 0 ) : array('status' => 103, 'amount' => 0 );
        }
        $comments = sprintf(t('The refund has been executed for the TID: %s with the amount of %s'), $transaction_call['tid'], uc_currency_format($transaction_call['refund_param']/100));
    }
    /* Parameter formation for amount booking process */
    elseif (!empty($form_values['nn_book_enable'])) {
        if (empty($form_values['nn_amount_book_textfield']) && $form_values['nn_amount_book_textfield'] <= 0) {
            form_set_error('nn_amount_book_textfield', t('The amount is invalid'));
            drupal_goto('admin/store/orders/' . $order->order_id);
        }
        $ext_process_type = 'book_amount';
        $transaction_call = unserialize($tid_details['request']); ## first call request

        if (!empty($transaction_call)) {
            unset($transaction_call['pan_hash'], $transaction_call['unique_id'], $transaction_call['create_payment_ref']);
            $update_variable = array('amount' => $form_values['nn_amount_book_textfield']);
            $enable_book_amount = 1;
            $transaction_call['payment_ref'] = $tid_details['tid'];
            $transaction_call['amount'] = $update_variable['amount'];
        }
    }
    
    /* Update the transaction in Novalnet transactio table */
    if (isset($update_variable)) {
        /* to send the extension process request to server */
        $response = uc_novalnet_send_payport($transaction_call, 'curl', $order->order_id);
        parse_str($response, $parsed);
        /* Checking the success status */
        if ($parsed['status'] == 100) {
            $transaction_value = unserialize($tid_details['masked_acc_details']);
            $transaction_value['paypal_transaction_id'] = isset($parsed['paypal_transaction_id']) ? $parsed['paypal_transaction_id'] : '';
            $transaction_value = serialize($transaction_value);
            update_variable(array('order_id' => $order->order_id), array('masked_acc_details' => $transaction_value), 'uc_novalnet_transaction');
            /* update the booked comments */
            if (!empty($enable_book_amount)) {
                $comments = sprintf(t("Your order has been booked with the amount of %s. Your new TID for the booked amount: %s"), uc_currency_format($update_variable['amount'] /100), $parsed['tid']);
                /* Update the transaction and amount on transaction success */
                update_variable(array('order_id' => $order->order_id), array('tid' => $parsed['tid'], 'amount' => $update_variable['amount']), 'uc_novalnet_transaction');
                /* Update the reference transaction on transaction success */
                update_variable(array('order_id' => $order->order_id), array('is_ref_order' => 0 ), 'uc_novalnet_transaction');
            }
            /* Update the transaction refund comments */
            if (($ext_process_type == 'refund') && !empty($parsed['tid']))
                $comments = sprintf(t('The refund has been executed for the TID: %s with the amount of %s. Your new TID for the refund amount: %s'), $transaction_call['tid'], uc_currency_format($transaction_call['refund_param']/100), $parsed['tid']);
            if (!empty($transaction_call['amount'])) {
                $_SESSION['novalnet_session'][$order->payment_method]['updated_amount'] = $transaction_call['amount']/100;
                $original_amount = uc_novalnet_get_original_amount($tid_details, $transaction_call['amount']);
                update_variable(array('order_id' => $order->order_id), array('masked_acc_details' => $original_amount), 'uc_novalnet_transaction');
                if (in_array($order->payment_method, array('novalnet_invoice', 'novalnet_prepayment'))) {
                    $transaction_call['amount'] = abs($transaction_call['amount'] - $tid_details['callback_amount']);
                }
                update_variable(array('order_id' => $order->order_id), array('amount' => $transaction_call['amount'], 'callback_total' => $transaction_call['amount'] ), 'uc_novalnet_transaction');
            }
            $comments     = callback_amt_update($order, $comments, ($ext_process_type == 'amount_update' && $tid_details['paymentkey'] == 27) ? $update_variable['due_date'] : '');
            $order_status = (($parsed['tid_status'] == '103') ? $basic_config['onhold_void_cancel_status'] : (!empty($order_status) ? $order_status : $order->order_status));
            /* Update the original status of the TID in Novalnet transaction*/
            update_variable(array('order_id' => $order->order_id), array('status' => $parsed['tid_status'] ), 'uc_novalnet_transaction');
            /* Updating the order comments */
            uc_order_comment_save($order->order_id,  0,  $comments ,  'order',  $order_status, 'TRUE');
            /* Updatig the order status */
            uc_order_update_status($order->order_id, $order_status);
            if (isset($_SESSION['novalnet_session'][$order->payment_method]['updated_amount']))
                unset($_SESSION['novalnet_session'][$order->payment_method]['updated_amount']);
            drupal_set_message($comments);
        }
        else isset($parsed['status_desc']) ? drupal_set_message($parsed['status_desc'], 'error') : '';
    }
}

/**
 *
 * Implements hook_mail().
 *
 * @params string $key
 * @params array $message
 * @params array $params
 * @return none
 */
function uc_novalnet_mail($key, &$message, $params) {
    $message['headers']['Content-Type'] = "text/html; charset=UTF-8; format=flowed; delsp=yes";
    $message['headers'] += $params['headers'];
    $message['subject']  = $params['subject'];
    $message['body'][]   = $params['body'];
}
/**
 *
 * Implements hook_mail_alter().
 *
 * @params array $message
 * @return none
 */
function uc_novalnet_mail_alter(&$message) {
    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    if ($message['id'] == 'uc_order_invoice' && $message['params']['order']->payment_method == 'novalnet_prepayment')
    $message['body'][0] = preg_replace('|<div id=nn_prepayment_comments>(.+?)</div>|s', ' ', $message['body'][0]);
}

/**
 *
 * Implements hook_cron().
 *
 * @params none
 * @return none
 */
function uc_novalnet_cron() {
    if ( date('d', variable_get('cron_last')) != date('d', time() ) ) {
        drupal_flush_all_caches();
    }
}


/**
 *
 * Displays the payment form in admin reorder page.
 *
 * @params object $order
 * @return array $build
 */
function uc_novalnet_admin_reorder($order) {
  $build['details']['order_total'] = array('#markup' => '<div><strong>' . t('Order total: @total', array('@total' => uc_currency_format($order->order_total))) . '</strong></div>');
  $build['details']['balance'] = array('#markup' => '<div><strong>' . t('Balance: @balance', array('@balance' => uc_currency_format(uc_payment_balance($order)))) . '</strong></div>');

  $build['form'] = drupal_get_form('uc_novalnet_admin_reorder_form', $order);

  return $build;
}

