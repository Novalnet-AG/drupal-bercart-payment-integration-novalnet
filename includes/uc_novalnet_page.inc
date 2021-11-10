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
 * Script   : uc_novalnet_page.inc
 *
 */

/**
 *
 * Callback script implementation : menu callback
 *
 * Menu callback that used to process the callback script
 *
 * @params none
 * @return none
 */
 
 define('LINEBREAK', empty($_SERVER['HTTP_HOST']) ? PHP_EOL : '<br />');
function uc_novalnet_callbackscript() {
	global $aryPayments, $aryChargebacks, $aryCollection, $aPaymentTypes;
	
	/** @Array Type of payment available - Level : 0 */
     $aryPayments = array('CREDITCARD', 'INVOICE_START', 'DIRECT_DEBIT_SEPA', 'GUARANTEED_DIRECT_DEBIT_SEPA', 'GUARANTEED_INVOICE', 'PAYPAL', 'ONLINE_TRANSFER', 'IDEAL', 'EPS', 'GIROPAY', 'PRZELEWY24', 'CASHPAYMENT');
    /** @Array Type of Chargebacks available - Level : 1 */
     $aryChargebacks = array('RETURN_DEBIT_SEPA', 'CREDITCARD_BOOKBACK', 'CREDITCARD_CHARGEBACK', 'REFUND_BY_BANK_TRANSFER_EU', 'PAYPAL_BOOKBACK', 'PRZELEWY24_REFUND', 'REVERSAL', 'CASHPAYMENT_REFUND','GUARANTEED_INVOICE_BOOKBACK' , 'GUARANTEED_SEPA_BOOKBACK');
    /** @Array Type of Credit entry payment and Collections available - Level : 2 */
     $aryCollection = array('INVOICE_CREDIT', 'CREDIT_ENTRY_CREDITCARD', 'CREDIT_ENTRY_SEPA', 'DEBT_COLLECTION_SEPA', 'DEBT_COLLECTION_CREDITCARD', 'ONLINE_TRANSFER_CREDIT', 'CASHPAYMENT_CREDIT','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE');
     $aPaymentTypes = array(
        'novalnet_invoice'     => array('INVOICE_START', 'INVOICE_CREDIT', 'SUBSCRIPTION_STOP', 'SUBSCRIPTION_REACTIVATE', 'GUARANTEED_INVOICE', 'REFUND_BY_BANK_TRANSFER_EU', 'GUARANTEED_INVOICE_BOOKBACK','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE'),
        'novalnet_prepayment'  => array('INVOICE_START', 'INVOICE_CREDIT', 'SUBSCRIPTION_STOP', 'SUBSCRIPTION_REACTIVATE', 'REFUND_BY_BANK_TRANSFER_EU'), 
        'novalnet_cashpayment' => array('CASHPAYMENT', 'CASHPAYMENT_CREDIT', 'CASHPAYMENT_REFUND'),
        'novalnet_paypal'      => array('PAYPAL', 'SUBSCRIPTION_STOP', 'SUBSCRIPTION_REACTIVATE', 'PAYPAL_BOOKBACK'), 
        'novalnet_instantbank' => array('ONLINE_TRANSFER', 'REFUND_BY_BANK_TRANSFER_EU', 'ONLINE_TRANSFER_CREDIT', 'REVERSAL','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE'),
        'novalnet_cc'          =>  array('CREDITCARD', 'CREDITCARD_BOOKBACK', 'CREDITCARD_CHARGEBACK', 'CREDIT_ENTRY_CREDITCARD', 'SUBSCRIPTION_STOP', 'SUBSCRIPTION_REACTIVATE', 'DEBT_COLLECTION_CREDITCARD'),
        'novalnet_sepa'        => array('DIRECT_DEBIT_SEPA', 'GUARANTEED_SEPA_BOOKBACK', 'RETURN_DEBIT_SEPA', 'SUBSCRIPTION_STOP', 'SUBSCRIPTION_REACTIVATE', 'DEBT_COLLECTION_SEPA', 'CREDIT_ENTRY_SEPA', 'REFUND_BY_BANK_TRANSFER_EU', 'GUARANTEED_DIRECT_DEBIT_SEPA'),
        'novalnet_ideal'       => array('IDEAL', 'REFUND_BY_BANK_TRANSFER_EU', 'ONLINE_TRANSFER_CREDIT', 'REVERSAL','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE'),
        'novalnet_eps'         => array('EPS', 'ONLINE_TRANSFER_CREDIT', 'REFUND_BY_BANK_TRANSFER_EU', 'REVERSAL','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE'),
        'novalnet_giropay'     => array('GIROPAY', 'ONLINE_TRANSFER_CREDIT', 'REFUND_BY_BANK_TRANSFER_EU', 'REVERSAL','CREDIT_ENTRY_DE','DEBT_COLLECTION_DE'),
        'novalnet_przelewy24'  => array('PRZELEWY24', 'PRZELEWY24_REFUND'),
        );
    
    /* Load the callback-script configuration from the Novalnet global configuration */
    $cbk_data = variable_get('novalnet_cbk_data');
    /* Fetch the callback values using request menthod*/
    $url_request = array_map('check_plain', $_REQUEST);
    /* remove the unneccessary spaces from the call back request */
    $url_request = array_map('trim', $_REQUEST);
    /* check the Test mode flag is activated */
    $cbk_data['test']  = (bool) $cbk_data['cbk_mail_test_mode'];
    /* Function to check the IP condition - if it is TRUE, further callback procees will take place */
    if (check_ipaddr($cbk_data, $url_request)) {
        $order = '';
        /* Function to check the callback request based on that process will organised(E.g: is recurring request etc.)*/
        if (check_params($cbk_data, $url_request, $order)) {
            set_order_status($url_request, $order, $cbk_data);
        }
    }
}

/**
 *
 * Callback script function : to check the Novalnet IP
 *
 * @params array $cbk_data
 * @return bool
 */
function check_ipaddr(&$cbk_data) {
    $real_host_ip = gethostbyname('pay-nn.de');
    $callback_ip  = ip_address();
    // Apply this exit/return as per your defined process in your vendor script! For eg: if defined as class, then return the error and handle the error message accordingly
    if(empty($real_host_ip)) {
       print 'Novalnet HOST IP missing';
       return FALSE;
    }
    if ($callback_ip !=  $real_host_ip && !$cbk_data['test']) {
        print 'Novalnet callback received' . ' ' . 'Unauthorised access from the IP [' . $callback_ip . ']';
        return FALSE;
    }
    return TRUE;
}

/**
 *
 * Callback script function : to organise the  url params
 *
 * @params array $cbk_data
 * @params array $url_request
 * @params object $order
 * @return bool
 */
function check_params($cbk_data, &$url_request, &$order) {
	global $aryPayments, $aryChargebacks, $aryCollection, $aPaymentTypes;
    /* Check the call back request has empty values */
    if (empty($url_request)) {
        show_message(t('Novalnet callback received. No params passed over!'));
    }
    /* Required params array */        
    $required_params = array('vendor_id','tid','payment_type','status','tid_status');
        $url_request['cbk_type'] = 'callback';
        $url_request['org_tid'] = $url_request['tid'];
       if (empty($url_request['vendor_activation'])) {
			if (in_array($url_request['payment_type'], array_merge($aryChargebacks, $aryCollection))) {                
                array_push($required_params, 'tid_payment');
                $url_request['org_tid'] = $url_request['tid_payment'];                
            }
	   }
	   else {
		   $url_request['cbk_type'] = 'affiliate';
		   $required_params = array(
            'vendor_id',
            'vendor_authcode',
            'product_id',
            'product_url',
            'activation_date',
            'aff_id',
            'aff_authcode',
            'aff_accesskey'
        );
	   }          
    
    $error_msg = '';
    foreach ($required_params as $val) {
        if ($url_request[$val] == '') {
            $error_msg .= t('Required param ( @val  ) missing!', array( '@val' => $val)) . LINEBREAK;
        }
        if (isset($url_request[$val]))
        $affiliate_params[$val] = $url_request[$val];
    }
    if (!empty($error_msg)) {
        show_message($error_msg);
        return FALSE;
    }
    /* Quits the validation of callback params, if type is affiliate */
    if ($url_request['cbk_type'] == 'affiliate') {
        $url_request = $affiliate_params;
        $url_request['cbk_type'] = 'affiliate';
        return TRUE;
    }
    /* Validate the callback params */
    $order = validate_param($url_request, $required_params, $error_msg);
    if (!$order) {
        show_message($error_msg);
        return FALSE;
    }
    else {
        if (!empty($_SESSION['nn']['nn_aff_id'])) unset($_SESSION['nn']['nn_aff_id']);
        $aff_details = nn_variable_get(array('customer_id' => $order->uid), 'nn_aff_user_detail');
        if (!empty($aff_details['aff_id'])) $_SESSION['nn']['nn_aff_id'] = $aff_details['aff_id'];
    }
    return TRUE;
}

/**
 *
 * Callback script function : to validate url params
 *
 * @params array $required_params
 * @params array $url_request
 * @params string $error_msg
 * @return mixed
 */
function validate_param($url_request, $required_params, &$error_msg) {
	global $aPaymentTypes;
	 $basic_params = variable_get('novalnet_params');
    foreach ($required_params as $key) {
        if (in_array($key, array('tid','signup_tid','tid_payment'))) {
            /* TID validation */
            if (! preg_match( '/^\d{17}$/', $url_request[$key])) {
                show_message(t('Novalnet callback received.') . ' ' . t('Invalid') . ucwords($key) . ' - ' . $url_request[$key]);
            }
        }
    }
    if (!empty($error_msg)) return FALSE;
    /* Load the order object using order id */
   
			$transdetails = db_select('uc_novalnet_transaction', 'n')->fields('n')->condition(db_or()->condition('order_id', $url_request['order_no'])->condition('tid', $url_request['org_tid']))->execute()->fetchAssoc();
			
			$order_no = $url_request['order_no'] ? $url_request['order_no'] : ($transdetails['order_id'] ? $transdetails['order_id'] : '');
			 $order = uc_order_load($order_no);
	
	// transaction cancelled for invoice and sepa payments		
	if (in_array($transdetails['status'], array('91','99','75', '98', '85')) && $url_request['payment_type'] == 'TRANSACTION_CANCELLATION') { 
		
		$comments = t('The transaction has been canceled on @date, @time', array('@date' => date('d-m-Y'), '@time' => date('H:i:s')));
		$order_status = $basic_params['onhold_void_cancel_status'];
		
		update_variable(array('tid' => $url_request['tid']), array('status' => $url_request['tid_status']), 'uc_novalnet_transaction');
		
		 uc_order_comment_save($order->order_id,  0, $comments ,  'order', $order_status , 'TRUE');		
    
		/* Updatig the order status */
		uc_order_update_status($order->order_id, $order_status);
		
		show_message($comments);
	}
     
      if(empty($transdetails) && (empty($order->order_id))) {
		criticalmail_comments($url_request);
	  }
	  
	  if (empty($transdetails['order_id']) && (in_array($url_request['payment_type'], array('ONLINE_TRANSFER', 'PAYPAL','EPS','GIROPAY','PRZELEWY24','IDEAL' )) || ($url_request['payment_type'] == 'CREDITCARD' && variable_get('novalnet_cc_cc3d'))) && (!empty($order->order_id))) { // If transaction not found in Novalnet table but the order number available in Novalnet system and payment temprorary id matches, handle communication break
               communication_failure($url_request);
         }
         
          /* Save the callback script request in Novalnet transaction log */
        uc_novalnet_save_log($transdetails['order_id'] , $url_request, 'callback');
        
        
    if (!empty($url_request['order_no']) && !empty($transdetails) && $transdetails['order_id'] != $url_request['order_no']) {
      show_message('Novalnet callback received. Order no is not valid!.');
      return FALSE;
    }
    
    
    if (!in_array($url_request['payment_type'], $aPaymentTypes[$order->payment_method])) {		
        show_message(t('Novalnet callback received.') . ' ' . t('Payment type Mismatched!'));
        return FALSE;
    }
    return $order;
}

/**
 *
 * Callback script function : to differentiate the callback types
 *
 * @params array $url_request
 * @params object $order
 * @params array $cbk_data
 * @return bool
 */
 
function set_order_status($url_request, $order, $cbk_data) {
	global $aryPayments, $aryChargebacks, $aryCollection, $aPaymentTypes;
	
	if ($url_request['cbk_type'] == 'affiliate') {
		activate_affiliate($url_request, $messages);
		show_message($messages);
	}
	$tid_details   = nn_variable_get(array('order_id' => $order->order_id), 'uc_novalnet_transaction');
	$url_request['amount'] = isset($url_request['amount']) ? $url_request['amount'] : 0;
    $order_balance = $tid_details['amount'];
    $currency = $order->currency;
    $amount   = sprintf('%0.2f', $url_request['amount']/100) . ' ' . $currency;
    $amt = isset($tid_details['callback_amount']) ? $tid_details['callback_amount'] + $url_request['amount'] : $url_request['amount'];
    $cbk_data = variable_get('novalnet_cbk_data');
    $messages = array('error' => '', 'mail_body' => '', 'comment' => '');
    $basic_params = variable_get('novalnet_params');
    
    if($url_request['cbk_type'] == 'callback') {
		if (in_array($url_request['payment_type'], $aryCollection)) {
			$order_state   = $order->order_status;
			if (in_array($url_request['payment_type'], array('CREDIT_ENTRY_CREDITCARD', 'DEBT_COLLECTION_CREDITCARD', 'CREDIT_ENTRY_SEPA', 'DEBT_COLLECTION_SEPA', 'GUARANTEED_DEBT_COLLECTION_SEPA', 'CREDIT_ENTRY_DE', 'DEBT_COLLECTION_DE'))) {
				$comment =  t('Novalnet Callback Script executed successfully for the TID: @tidpayment with amount @amount on @date @time. Please refer PAID transactions in our Novalnet Admin Portal with the TID @tid.', array('%s' => ':', '@tidpayment' =>$url_request['tid_payment'], '@date' => date('d-m-Y'), '@time' => date('H:i:s'), '@tid' => $url_request['tid'], '@amount' => sprintf('%0.2f', $url_request['amount']/100) . ' ' . $currency)) . LINEBREAK;
			}
			if (in_array($url_request['payment_type'], array('INVOICE_CREDIT','CASHPAYMENT_CREDIT','ONLINE_TRANSFER_CREDIT'))) {
				
                if ( 'ONLINE_TRANSFER_CREDIT' === $url_request['payment_type'] ) {

                    /* translators: %s: amount,order_no */                   
                    $comment = t('Novalnet Callback Script executed successfully for the TID: @tidpayment with amount @amount on @date @time. Please refer PAID transactions in our Novalnet Admin Portal with the TID @tid.', array('%s' => ':', '@tidpayment' =>$url_request['tid_payment'], '@date' => date('d-m-Y'), '@time' => date('H:i:s'), '@tid' => $url_request['tid'], '@amount' => sprintf('%0.2f', $url_request['amount']/100) . ' ' . $currency)) . LINEBREAK;

                    $order_state   =  variable_get($order->payment_method . '_after_status', 'completed');



                } else if ($order_balance > 0) {
					$comment =  t('Novalnet Callback Script executed successfully for the TID: @tidpayment with amount @amount on @date @time. Please refer PAID transactions in our Novalnet Admin Portal with the TID @tid.', array('%s' => ':', '@tidpayment' =>$url_request['tid_payment'], '@date' => date('d-m-Y'), '@time' => date('H:i:s'), '@tid' => $url_request['tid'], '@amount' => sprintf('%0.2f', $url_request['amount']/100) . ' ' . $currency));
					
					$order_bal = $order_balance - $url_request['amount'];
               
					if ($order_bal <= 0) {
						$order_state   =  variable_get($order->payment_method . '_after_status', 'completed');
						update_variable(array('order_id' => $order->order_id), array('amount' => $order_balance), 'uc_novalnet_transaction');
					}
					if ($order_bal < 0) {
						$comment .= t('Customer paid amount is greater than the order amount.');
					}
				} 
				else {
					show_message(t('Novalnet callback received. Callback Script executed already'));
					return FALSE;
				}
			}
			$order_balance_check = isset($order_bal) ? $order_bal : $url_request['amount'];
			update_variable(array('order_id' => $order->order_id), array('amount' => $order_balance_check, 'callback_amount' => $amt), 'uc_novalnet_transaction');
			uc_order_comment_save($order->order_id,  0,  $comment ,  'order',  $order_state, 'TRUE');
			uc_payment_enter($order->order_id, $order->payment_method, ($url_request['amount']/100),  $order->uid,  NULL, $comment);
			uc_order_update_status($order->order_id, $order_state);
			uc_novalnet_cbk_mail($cbk_data, $comment);
			show_message($comment);
		}
		
		if (in_array($url_request['payment_type'], $aryChargebacks)) {

			$comment = in_array($url_request['payment_type'], array('PAYPAL_BOOKBACK','CREDITCARD_BOOKBACK','PRZELEWY24_REFUND','GUARANTEED_INVOICE_BOOKBACK','GUARANTEED_SEPA_BOOKBACK','CASHPAYMENT_REFUND','REFUND_BY_BANK_TRANSFER_EU')) ? t("Novalnet callback received. Refund/Bookback executed successfully for the TID: @tidpayment amount: @amount on @date. The subsequent TID: @tid", array('%s' => ':', '@tidpayment' => $url_request['org_tid'], '@amount' => $amount, '@date' => date('d-m-Y'), '@tid' => $url_request['tid'])) : t("Novalnet callback received. Chargeback executed successfully for the TID: @tidpayment amount: @amount on @date. The subsequent TID: @tid", array('%s' => ':', '@tidpayment' => $url_request['org_tid'], '@amount' => $amount, '@date' => date('d-m-Y'), '@tid' => $url_request['tid']));
			
			$result = nn_variable_get(array('order_id' => $order->order_id), 'uc_payment_receipts', array('comment'));
			update_variable(array('order_id' => $order->order_id), array('callback_amount' => $amt), 'uc_novalnet_transaction');
			uc_order_comment_save($order->order_id,  0,  $comment ,  'order',  $order->order_status, 'TRUE');
			uc_novalnet_cbk_mail($cbk_data, $comment);
			show_message($comment);
		}
		if (in_array($url_request['payment_type'], $aryPayments)) {
			if (in_array($url_request['payment_type'], array('PAYPAL', 'PRZELEWY24')) && $url_request['tid_status'] == '100'){
				if($tid_details['callback_amount'] >= 0) {

                    $comment =  t('Novalnet Callback Script executed successfully for the TID: @tid with amount @amount on @date @time.', array('@date' => date('d-m-Y'), '@time' => date('H:i:s'), '@tid' => $url_request['tid'], '@amount' => $amount)) . LINEBREAK;

					$order_state   = variable_get($order->payment_method . '_after_status', 'completed');
					$order_bal = $order_balance = $url_request['amount'];
					$order_balance_check = isset($order_bal) ? $order_bal : $url_request['amount'];
					
					uc_order_update_status($order->order_id, $order_state);
					uc_order_comment_save($order->order_id,  0,  $comment ,  'order',  $order_state, 'TRUE');
					update_variable(array('order_id' => $order->order_id), array('status' => $url_request['tid_status'], 'amount' => $order_balance_check, 'callback_amount' => $amt), 'uc_novalnet_transaction');				
					uc_novalnet_cbk_mail($cbk_data, $comment);
				}
				else {
					$comment = t('Novalnet callbackscript received. Order already paid!');
				}
				show_message($comment);
			}
			if ($url_request['payment_type'] == 'PRZELEWY24' && $url_request['tid_status'] != '100') {
				$message          = (($url_request['status_message']) ? $url_request['status_message'] : (($url_request['status_text']) ? $url_request['status_text'] : (($url_request['status_desc']) ? $url_request['status_desc'] : t('Payment was not successful. An error occurred.'))));
				
                $messages['mail_body'] = t('The transaction has been canceled due to%s ', array('%s' => ':')) . $message;
                
                $result = nn_variable_get(array('order_id' => $order->order_id), 'uc_payment_receipts', array('comment'));
				update_variable(array('order_id' => $order->order_id), array('status' => $url_request['tid_status']), 'uc_novalnet_transaction');
				uc_order_comment_save($order->order_id,  0,  $result['comment'] . PHP_EOL . $messages['mail_body'] ,  'order',  $order->order_status, 'TRUE');
				uc_novalnet_cbk_mail($cbk_data, $comment);
				show_message($comment);
					
				}
				 if (in_array($url_request['payment_type'], array('INVOICE_START','GUARANTEED_INVOICE','DIRECT_DEBIT_SEPA','GUARANTEED_DIRECT_DEBIT_SEPA', 'CREDITCARD', 'PAYPAL')) && in_array($tid_details['status'], array(75,85,91,98,99)) && in_array($url_request['tid_status'], array(91,99,100)) && $url_request['status'] == '100') { 
					 if(in_array($url_request['tid_status'], array(91,99,100)) && in_array($tid_details['status'], array(91,85,98,99,75))) {

						 $comment = '';
                         $order_state = ($tid_details['status'] == 91 && $url_request['tid_status'] == 100 && $url_request['payment_type'] == 'INVOICE_START') ? variable_get($order->payment_method . '_before_status', 'completed') : variable_get($order->payment_method . '_after_status', 'completed') ;
						if($url_request['payment_type'] == 'GUARANTEED_INVOICE') {
							$order_state = ($tid_details['status'] == 75 && $url_request['tid_status'] == 91) ? $basic_params['onhold_transaction_status'] : variable_get($order->payment_method . '_after_status', 'completed');
						}
						if($url_request['payment_type'] == 'GUARANTEED_DIRECT_DEBIT_SEPA') {
							$order_state = ($url_request['tid_status'] == 99) ? $basic_params['onhold_transaction_status'] : variable_get($order->payment_method . '_after_status', 'completed');
						}
						if(in_array($tid_details['status'], array('91', '75')) && in_array($url_request['payment_type'], array('INVOICE_START', 'GUARANTEED_INVOICE')) && in_array($url_request['status'], array('91', '100'))) {						
    						$comment =  t('Novalnet transaction ID%s', array('%s' => ':')).$url_request['tid'].LINEBREAK.t('Please transfer the amount to the below mentioned account details of our payment processor Novalnet') .LINEBREAK. t('Due date: @due_date', array('@due_date' => isset($url_request['due_date']) ? $url_request['due_date'] : '')) . LINEBREAK.
    						t('Account holder: @comp', array('@comp' => isset($url_request['invoice_account_holder']) ? $url_request['invoice_account_holder'] : '')) . LINEBREAK .
    						t('IBAN: @invoice_iban', array('@invoice_iban' => isset($url_request['invoice_iban']) ? $url_request['invoice_iban'] : '')) . LINEBREAK .
    						t('BIC: @invoice_bic', array('@invoice_bic' => isset($url_request['invoice_bic']) ? $url_request['invoice_bic'] : '')) . LINEBREAK .
    						t('Bank: @bank', array('@bank' => isset($url_request['invoice_bankname']) ? $url_request['invoice_bankname'] : $url_request['invoice_bankname']. ' ' . isset($url_request['invoice_bankplace']) ? $url_request['invoice_bankplace'] : '')) . LINEBREAK .
    						t('Amount: @amount', array('@amount' => uc_currency_format($amount))) . LINEBREAK;
    						$comment .= t('Please use any one of the following references as the payment reference, as only through this way your payment is matched and assigned to the order:') . '<br/>';
    						$comment .= t("Payment Reference 1") . ': ' . (isset($url_request['invoice_ref']) ? $url_request['invoice_ref'] : '') . LINEBREAK;
    						$comment .= t("Payment Reference 2") . ': ' . t('TID') . ' ' . $url_request['tid'] . LINEBREAK;
						}
						$comment .= (in_array($tid_details['status'], array('75','85','91','98','99')) && $url_request['tid_status'] == 100) ? t('The transaction has been confirmed on @date, @time', array('@date' => date('d-m-Y'), '@time' => date('H:i:s'))) : t('The transaction status has been changed from pending to hold for the TID: @tid on @date', array( '@tid' => $url_request['tid'], '@date' => date('d-m-y')));
						
    					update_variable(array('order_id' => $order->order_id), array('status' => $url_request['tid_status']), 'uc_novalnet_transaction');
    					uc_order_comment_save($order->order_id,  0,  $comment ,  'order',  $order_state, 'TRUE');    
    					uc_order_update_status($order->order_id, $order_state);
    					uc_novalnet_cbk_mail($cbk_data, $comment);
    					show_message($comment);
					}	
					
				}
				else {
					$comment = t('Novalnet callback received. Payment type ( @payment_type ) is
                    not applicable for this process!', array('@payment_type' => $url_request['payment_type']));
                    show_message($comment);
				}
		}
	}
    else {
        /* Function name manipulated as per the callback script request type*/
        $status_var = ($url_request['status'] != 100) ? 'Status' : 'TID status';
        $messages['error'] .= t('Novalnet callback received.') . ' ' . t("$status_var is not valid: Only 100 is allowed") . LINEBREAK;
    }
    if (!empty($messages['error'])) {
        show_message($messages['error']);
        return FALSE;
    }
    
}

/**
 *
 * Callback script function : to activate affiliate process
 *
 * Activate the affiliate process - by inserting affiliate params in affiliate table.
 *
 * @params array $affiliate_params
 * @params array $messages
 * @return none
 */
function activate_affiliate($affiliate_params, &$messages) {
    unset($affiliate_params['cbk_type']);
    $affiliate_params['activation_date'] = date('Y-m-d H:i:s', strtotime($affiliate_params['activation_date']));
    update_variable('', $affiliate_params, 'nn_aff_account_detail');
    $messages .= LINEBREAK . 'Novalnet callback script executed successfully with Novalnet account activation information.' . LINEBREAK;
}

/**
 *
 * Callback script function : For callback notification mail
 *
 * Drupal mails to procees the callback notification mails
 *
 * @params array $cbk_data
 * @params array $messages
 * @return none
 */
function uc_novalnet_cbk_mail($cbk_data, $messages, $missing_transaction_notify = false) {	
    global $language;
    if ($missing_transaction_notify) { //This is only for missing transaction notification
		$cbk_data['cbk_mail_body'] = LINEBREAK;
		$cbk_data['cbk_mail_body'] = $messages;
		$cbk_data['cbk_mail_to']     = 'technic@novalnet.de';
		$params['subject'] = 'Critical error on shop system ' . variable_get('uc_store_name') . ': order not found for TID: ' . $cbk_data['tid'];
		$params['body'] = $cbk_data['cbk_mail_body'];
		$params['headers'] = array();
		$cbk_data['cbk_mail_from'] = variable_get('uc_store_email');
		$mailreslt = drupal_mail('uc_novalnet', 'nn_callback', $cbk_data['cbk_mail_to'], $language -> language, $params, $cbk_data['cbk_mail_from'], TRUE);
        }
        
	else {
		if(($cbk_data['cbk_mail_send'] == 0 && !empty($cbk_data['cbk_mail_to']))) {
		$cbk_data['cbk_mail_body'] .= LINEBREAK;
		$cbk_data['cbk_mail_body'] .= $messages;
		$params['subject'] = $cbk_data['cbk_mail_subject'];
		$params['body'] = $cbk_data['cbk_mail_body'];
		$params['headers'] = array(
        'Bcc' => $cbk_data['cbk_mail_bcc']);
        $mailreslt = drupal_mail('uc_novalnet', 'nn_callback', $cbk_data['cbk_mail_to'], $language -> language, $params, $cbk_data['cbk_mail_from'], TRUE);
	}
    
	}	
	
    if ($mailreslt['result'] == 1) show_message('<meta
    content="text/html; charset=utf-8" http-equiv="Content-Type">'
    . t('Sending Email succeeded!') . $cbk_data['cbk_mail_body'] . LINEBREAK . $messages);
}

/**
 *
 * Callback script function : new order generation after the recurring runs
 * (hook function to create a new order)
 *
 * @params object $old_order
 * @return object $order
 */
function create_order($old_order) {
    $node_id = end($old_order->products);
    $productnode = node_load($node_id->nid);
    $product = db_select('uc_products', 'c')->fields('c')->condition('vid', $productnode->vid)->execute()->fetchAll();
    $product = $product[0];
    $product->qty       = $old_order->product_count;
    $product->title     = $productnode->title;
    $product->nid       = $productnode->nid;
    $product->price     = $productnode->sell_price;
    $product->data['shippable'] = $product->shippable;

    $order = uc_order_new($old_order->uid, $old_order->order_status);
    foreach (array('delivery_first_name', 'delivery_last_name', 'delivery_phone', 'delivery_company', 'delivery_street1', 'delivery_street2', 'delivery_city', 'delivery_zone', 'delivery_postal_code', 'delivery_country', 'primary_email', 'billing_first_name', 'billing_last_name', 'billing_company', 'billing_phone', 'billing_city', 'billing_street1', 'billing_street2', 'billing_postal_code', 'billing_zone', 'billing_country') as $val) {
        $order->$val = $old_order->$val;
    }
    for ($i = 0; $i <= $old_order->product_count; $i++) {
      $order->products[] = $product;
    }
    $order->payment_method = $old_order->payment_method;
    foreach ($old_order->line_items as $item) {
        if ($item['type'] != 'subtotal')
            $order->line_items[] = uc_order_line_item_add($order->order_id, $item['type'], $item['title'], $item['amount']);
    }
    uc_order_save($order);
    $order = uc_order_load($order->order_id);
    return $order;
}

function communication_failure($url_request) {

		$order = uc_order_load($url_request['order_no']);
        $payment_method = $url_request['payment_type'] == 'CREDITCARD' ? 'cc' : ($url_request['payment_type'] == 'ONLINE_TRANSFER' ? 'instantbank' : strtolower($url_request['payment_type']));
        $url_request['payment_method'] = 'novalnet_'.$payment_method;
        $getkey = uc_novalnet_payment_methods($url_request['payment_method']);
        $url_request['key'] = $getkey['key'];
	if ((in_array($url_request['status'],array('100','90'))) && in_array($url_request['tid_status'], array(100,90,85,86,91,98,99,75))) { // Handle success transaction
	
		$note = t('Novalnet Callback Script executed successfully') . LINEBREAK;
        $note .= t('Novalnet transaction ID%s', array('%s' => ':')). $url_request['tid'] . LINEBREAK;
        if ($url_request['test_mode'] == 1) { // If test transaction
			$note .= t('Test order') . LINEBREAK;
        }
        $order = uc_order_load($url_request['order_no']);
        $payment_method = $url_request['payment_type'] == 'CREDITCARD' ? 'cc' : ($url_request['payment_type'] == 'ONLINE_TRANSFER' ? 'instantank' : strtolower($url_request['payment_type']));
        $url_request['payment_method'] = 'novalnet_'.$payment_method;
         $order_status = variable_get($url_request['payment_method']. '_after_status', 'completed');
	}
	else {
		$note = t('Novalnet Callback Script executed successfully') . LINEBREAK;
        $note .= t('Novalnet transaction ID%s', array('%s' => ':')). $url_request['tid'] . LINEBREAK;
        if ($url_request['test_mode'] == 1) { // If test transaction
			$note .= t('Test order') . LINEBREAK;
        }
            $note .= (($url_request['status_message']) ? $url_request['status_message'] : (($url_request['status_text']) ? $url_request['status_text'] : (($url_request['status_desc']) ? $url_request['status_desc'] : 'Payment was not successful. An error occurred'))) .LINEBREAK;
            
            
         $order_status = variable_get($url_request['payment_method']. '_after_status', 'completed');
        
	}
	 uc_order_comment_save($order->order_id,  0, $note ,  'order', $order_status , 'TRUE');
         /* Adding the line items in order history */
    uc_payment_enter($order->order_id,  $url_request['payment_method'] ,  $order->order_total,  $order->uid,  NULL,  $note);
    
    /* Updatig the order status */
    uc_order_update_status($order->order_id, $order_status);
    uc_novalnet_update_status($url_request);
    show_message($note);
	
}
/**
 *
 * Callback script function : to print the callback message
 *
 * @params string $str
 */
function show_message($str) {
        echo 'message= '.$str;
    exit;
}

function criticalmail_comments($url_params) {
	
        $comments = t('Dear Technic team,'). '<br />' . LINEBREAK;
        $comments .= t('Please evaluate this transaction and contact our payment module team at Novalnet.') . LINEBREAK . LINEBREAK;
        $comments .= strip_tags(t('Merchant ID%s ', array('%s' => ':'))) . $url_params['vendor_id'] . LINEBREAK;
        $comments .= strip_tags(t('Project ID%s ', array('%s' => ':'))) . (isset($url_params['product_id']) ? $url_params['product_id'] : '' ). LINEBREAK;
        $comments .= strip_tags(t('TID%s ', array('%s' => ':'))) . $url_params['tid']. LINEBREAK;
        $comments .= strip_tags(t('TID status%s ', array('%s' => ':'))) . $url_params['tid_status'] . LINEBREAK;
        $comments .= strip_tags(t('Order no%s ', array('%s' => ':'))) . (isset($url_params['order_no'])  ? $url_params['order_no'] : '' ). LINEBREAK;
        $comments .= strip_tags(t('Payment type%s ', array('%s' => ':'))) . $url_params['payment_type'] . LINEBREAK;
        $comments .= strip_tags(t('E-mail%s ', array('%s' => ':'))) . (isset($url_params['email'])  ? $url_params['email'] : ''). LINEBREAK;
        
       uc_novalnet_cbk_mail(array('tid' => $url_params['tid']), $comments, true);
}
