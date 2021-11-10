/**
 * @file
 * Script for uc_novalnet module.
 **/
(function($) {
$(document).ready(function() {	
           $('#publicKey').change(function() {
						jQuery.ajax({
                 type: 'POST',
                    url: 'autofill_paymentdata',
                    dataType: 'json',
                    data: {'api_config_hash' : $('#publicKey').val()},
                    success: function(response) {
						if(response.status == '100') {
							$('#vendor_id').val(response.data['vendor']);
							$('#auth_code').val(response.data['auth_code']);
							$('#product_id').val(response.data['product']);
							$('#payment_access_key').val(response.data['access_key']);	
							$.each(response.data['tariff'], function(key, value) {
								if(value.type == '2' || value.type == '3') {
									$('<option>').val(key).text(value.name).appendTo('#tariff_id');
								} 
							});						
						}
						else if(response.status == '1') {
						$('#publicKey, #vendor_id, #auth_code, #product_id,#payment_access_key').val('');
						$('#tariff_id').find('option').remove();
						alert(response.data['config_result']);
						}
						else {
							$('#publicKey, #vendor_id, #auth_code, #product_id,#payment_access_key').val('');
						$('#tariff_id').find('option').remove();
						}
						},
					});
            });
    
    if($('#nnsepa_ibanconf_bool').val() == 1){
		$('#nnsepa_ibanconf_desc').hide();
	}
	
	$('#nnsepa_ibanconf').click(function (event) {
            event.preventDefault();
            if ($('#nnsepa_ibanconf_bool').val() == 1) {
              $('#nnsepa_ibanconf_desc').show();
              $('#nnsepa_ibanconf_bool').val(0);
              
            }
            else {
               $('#nnsepa_ibanconf_desc').hide();
               $('#nnsepa_ibanconf_bool').val(1);
              
            }
              
          });
	  
    $('#edit-submit').click(function(){
        $('#edit-submit').hide();
    });

    if (jQuery('#nn_admin_payment').val() != 'novalnet_cc') {
        $('#edit-submit').click(function(){
            $('#edit-submit').hide();
        });
    }

    if ($('#nn_cc_hash_generated').val() == 1 && $('#one_shop_cc_enabled').val() == 1) {
        $('#loading_image').hide();
        $('#load_cc_iframe').hide();
    }
    /* For birth date field adjustment */
    $('.container-inline').css({'width': '35%', 'float': 'left'});

    /* special character validation */
    $('#nnsepa_owner, #nnsepa_acno').live('keypress',function(e) {
        return validate_account_number(e);
    });
     /* special character validation */
    $('#edit-transaction-amount-change').live('keypress',function(e) {
        return number_validate(e);
    });

    /* number validation */
    $('#nn_amount_book_textfield, #edit-transaction-refund').live('keypress',function(e) {
        return number_validate(e);
    });

    $('#edit-submit').click(function() {
        var display_text = '';
        if($('#nn_amount_book_textfield').val() != '' && $("#nn_book_enable").is(":checked")) {
            display_text = Drupal.t('Are you sure you want to book the order amount?');
        }
        if($("#enable_nn_refund").is(":checked") && $('.form-item-transaction-refund').css('display') == 'block') {
            display_text = Drupal.t('Are you sure you want to refund the amount?');
        }
        if($('.form-item-transaction-amount-change').css('display') == 'block' && $("#enable_amount_update").is(":checked") && $('input[name="invoice_duedate_change"]').val() != '' && $('input[name="invoice_duedate_change"]').val() != undefined) {
            display_text = Drupal.t('Are you sure you want to change the order amount or due date?');
        }
        if ($('.form-item-transaction-amount-change').css('display') == 'block' && $("#enable_amount_update").is(":checked") && ($('input[name="invoice_duedate_change"]').val() == '' || $('input[name="invoice_duedate_change"]').val() == undefined) && !$("#transaction_enable").is(":checked")) {
            display_text = Drupal.t('Are you sure you want to change the order amount?');
        }
        if($('.form-item-on-hold-transaction').css('display') == 'block' && $("#transaction_enable").is(":checked")) {
            if($('select[name="on_hold_transaction"] option:selected').val() == 'capture' ) {
                display_text = Drupal.t('Are you sure you want to capture the payment?');
            } else if($('select[name="on_hold_transaction"] option:selected').val() == 'void' ) {
                display_text = Drupal.t('Are you sure you want to cancel the payment?');
            }
        }
        if(display_text != '') {
            if (!confirm(display_text)) {
                $('#edit-submit').show();
                return false;
            }
            return true;
        }
        
    });
    
    /* mode changes for extensions (for hiding the extension with one another) */
    $('#transaction_enable, #enable_amount_update, #enable_nn_refund').click(function() {
        enable_mode(this);
    });
    /* Fetch the payment name from the payment selection */
    var payment = $('input[name="panes[payment][payment_method]"]:checked').val();
    /* Ajax complete function */
    $( document ).ajaxComplete(function( event, xhr, settings ) {
    /* Fetch the payment name from the payment selection */
    var payment = $('input[name="panes[payment][payment_method]"]:checked').val();
    var re = /system\/ajax/gi;
        if (re.test(settings.url)) {
            /* For birth date field adjustment */
            $('.container-inline').css({'width': '35%', 'float': 'left'});
                if (payment == 'novalnet_sepa'){
                    /* Load the one step shopping functionality for sepa*/
                    oneshop_sepa_load();
                }
                if (payment == 'novalnet_paypal' && $('#one_shop_paypal_enabled').val() == 1 && $('.given_paypal_details').css('display') == 'block') {
                    $("#cc_desc").hide();
                }
        }
    });

    if (payment == 'novalnet_sepa') {
            /* load the customer details on information change */
        $.each( [ 'nnsepa_owner', 'nnsepa_country', 'nnsepa_acno'], function( i, keyVal ) {
            $('#' + keyVal ).live('change', function () {
                $('#nnsepa_ibanconf').attr('checked', false);
            });
        });
    }
    if($('input[name="panes[payment][payment_method]"]:checked').val() == 'novalnet_sepa' && $('#one_shop_sepa_enabled').val() == 1 && ( $('.given_sepa_details').css('display') == 'block' || $('.given_sepa_details').css('display') == 'inline')) {
        $(".form-item-panes-payment-details-nnsepa-ibanconf").hide();
    } else if($('input[name="panes[payment][payment_method]"]:checked').val() == 'novalnet_paypal' && $('#one_shop_paypal_enabled').val() == 1 && $('.given_paypal_details').css('display') == 'block') {
        $("#cc_desc").hide();
    }

    if ($('#nn_admin_payment').val() == 'novalnet_paypal' && $('#edit-continue').val() != '') {
        $('#edit-continue').click(function(e) {
            if ($('.paypal_given_details').val() == 0) {
                $("#paypal_hidden").submit();
            } else if ($('.paypal_given_details').val() == 1) {
                $('#paypal_direct_hidden').submit();
            }

        });
    }

    /* Checking one shop clicking enable on page loads */
    oneshop_sepa_load();
    $('#nn_book_enable').click(function() {
        $(".form-item-nn-amount-book-textfield").toggle("disabled");
    });

    $(document).ajaxComplete(function(event, request, settings){
        var payment = ($('input[name="panes[payment][payment_method]"]:checked').length) ? $('input[name="panes[payment][payment_method]"]:checked').val() : $('#nn_admin_payment').val();
        if (payment == 'novalnet_cc') {
            if(($('#nn_cc_hash_generated').val() == 1) && $('#one_shop_cc_enabled').val() == 1){
               $('#loading_image, #load_cc_iframe').css('display', 'none');
            }
        }
    });

    /* one step checkout process for credit card */
    $(".cc_enter_new" ).live('click', function(e) {

           if($(this).text() == Drupal.t('Enter new card details')){
                ccloadiframe(JSON.stringify({callBack: 'getHeight'}))
                $('#load_cc_iframe').show();
                $("#one_page_shopping, #nn_cc_hash_generated").val(0)
                new_card_details();
            } else {
                $("#one_page_shopping, #nn_cc_hash_generated").val(1)
                $('#load_cc_iframe, #redirect_desc').hide();
                $(".cc_enter_new").text(Drupal.t('Enter new card details'));
                $(".given_cc_details, #cc_desc").show();
            }
    });
    /* Load the Creditcard payment form when click the enter new card details */
    function new_card_details() {
        $(".cc_enter_new").text(Drupal.t('Given card details')); //Enter new card details
        $(".given_cc_details").hide();
    }
    /* one step checkout process for Paypal */
    $(".paypal_enter_new" ).live('click', function(e) {
        if($(this).text() == Drupal.t('Proceed with new PayPal account details')) {
            $('.paypal_one_div, .given_paypal_details').hide();
            $(".paypal_given_details").attr("value", "0");
            $("#cc_desc").show();
            new_paypal_details();
        } else {
            $("#one_shop_paypal_enabled").val(1)
            $(".paypal_given_details").attr("value", "1");
            $(".paypal_enter_new").text(Drupal.t('Proceed with new PayPal account details'));
            $(".given_paypal_details").show();
            $("#cc_desc").hide();
        }
    });
    /* Load the Paypal payment form when click the Proceed with new PayPal account details */
    function new_paypal_details() {
        $(".paypal_enter_new").text(Drupal.t('Given PayPal account details'));
        $(".given_paypal_details").hide();
    }

    $(".sepa_enter_new" ).live('click', function(e) {
        /* if the user want to enter the new accout details */
        if($(this).text() == Drupal.t('Enter new account details')){
            $(".form-item-panes-payment-details-nnsepa-ibanconf").show();
            $('#nnsepa_ibanconf').show();
            $(".form-item-panes-payment-details-nnsepa-ibanconf").css('margin-left', '29%');
            $('#nnsepa_ibanconf').attr('checked', false);
            new_account_details();
        } else {
            /* Change the text */
            $(".sepa_enter_new").text(Drupal.t('Enter new account details'));
            /* Show the masking form */
            $('.given_sepa_details').show();
            $('#nnsepa_ibanconf').attr('checked', false);
            $(".form-item-panes-payment-details-nnsepa-ibanconf").hide();
            /* Hide the sepa payment form */
            $('.sepa_one_div, .form-item-panes-payment-details-novalnet-sepa-pinkey').hide();
            $(".sepa_given_details").val(0);
            /* Hide the Pin field */
        }
    });
});



/* Response handler function */
function check_result(response, ptype, qryString, from_iban) {
    if (ptype =="api_config" && response != '') {
        $("#nn_global_loading").hide();
        $("#content").css("opacity","1");
        $("#nn_global_loading").attr('data-val', 1);
        window.location.href = $('#base_url').val() +"admin/novalnet/globalconfig";
        return true;
     }
    var result = $.parseJSON(response);
    if (result.hash_result != 'success') {
        $('#loading-img').hide();
        alert(result.hash_result);
         $('#nnsepa_ibanconf').attr('checked', false);
        return false;
    }
}

function sent_xdomainreq(qryString, ptype, from_iban){
    if (from_iban != 'hash')
    qryString = $.param( qryString );

    var nnurl = "https://cdn.barzahlen.de/js/v2/checkout-sandbox.js";
    if ('XDomainRequest' in window && window.XDomainRequest !== null) {
        var xdr = new XDomainRequest();
        xdr.open('POST', nnurl);
        xdr.onload = function () {
            return check_result(this.responseText, ptype, qryString , from_iban);
        }
        xdr.onerror = function() {
            _result = false;
        };
        xdr.send(qryString);
    } else{
        var xmlhttp = (window.XMLHttpRequest) ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHTTP");
        xmlhttp.onreadystatechange=function() {
            if (xmlhttp.readyState==4 && xmlhttp.status==200){
                return check_result( xmlhttp.responseText, ptype, qryString , from_iban);
            }
        }
        xmlhttp.open("POST",nnurl,true);
        xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xmlhttp.send(qryString);
    }
}
/* Remove unwanted special characters from Card holder and account holder names */
function remove_unwanted_special_chars(input_val) {
    if( input_val != undefined || input_val != ''){
        return input_val.replace(/[\/\\|\]\[|#,+()$~%":`'@~;*?<>!^{}=_]/g,'');
    }
}
/* validate the unwanted special characters */
function validate_special_chars(input_val){
    var re = /[\/\\#,+@!^()&$~%.":*?<>{}]/g;
    return re.test(input_val);
}
/* Process mode handling on extension process */
function enable_mode(mode){
    var field = $(mode).data('text');
    var refund = $(mode).data('refund');
    $("#"+field, "#"+refund).toggle("disabled");
}
/* validate the account numbers */
function validate_account_number(event){
    var keycode = ('which' in event) ? event.which : event.keyCode;
    var reg = /^(?:[A-Za-z0-9]+$)/;

    if(event.target.id == 'nnsepa_owner') reg = /[^0-9\[\]\/\\#,+@!^()$~%'"=:;<>{}\_\|*°§?`]/g;
    return (reg.test(String.fromCharCode(keycode)) || keycode == 0 || keycode == 8) ? true : false;
}
 /* Load the one step shopping functionality for sepa*/
function oneshop_sepa_load() {
    if ($("#one_shop_sepa_enabled").val() == 1) {
        $(".sepa_one_div, .form-item-panes-payment-details-novalnet-sepa-pinkey").hide();
        if ( $('#form_error_type_sepa').val() == 1)
            new_account_details();
    }
}

/* Get the customer details on information change */
function get_name(){
    var name = ( ( $('#edit-panes-billing-copy-address').attr('checked') == false || $('#edit-panes-billing-copy-address').attr('checked') ==undefined ) ? ($("input[name=\'panes[billing][billing_first_name]\']").val() + ' ' + $("input[name=\'panes[billing][billing_last_name]\']").val()) : ($("input[name=\'panes[delivery][delivery_first_name]\']").val() + ' ' + $("input[name=\'panes[delivery][delivery_last_name]\']").val()) ) ;
    $('#nncc_owner').val(name);
}

/* Load the SEPA payment form when end-user provide the new card details */
function new_account_details() {
    /* Change the text */
    $(".sepa_enter_new").text(Drupal.t('Given account details'));
    /* Hide the masking form */
    $('.given_sepa_details').hide();
    /* Show the payment form */
    $('.sepa_one_div, .form-item-panes-payment-details-novalnet-sepa-pinkey').show();
    $(".sepa_given_details").val(1);
}

function number_validate(event) {
    var keycode = ('which' in event) ? event.which : event.keyCode;
    return (/^(?:[0-9\s]+$)/.test(String.fromCharCode(keycode)) || keycode == 0 || keycode == 8);
}

})(jQuery);

// CC start
function load_cc_iframe() {

    jQuery('#loading_image').hide();
    var iframe = document.getElementById('nnIframe').contentWindow;
    var targetOrigin = 'https://secure.novalnet.de';
    var styleObj = {
                        labelStyle : jQuery('#nn_label').val(),
                        inputStyle : jQuery('#nn_input').val(),
                        styleText  : jQuery('#nn_css_text').val(),
                    };
    var textObj = {
					card_holder : {
					cardHolderErrorText : '',
					},
					card_number : {
					cardNumberErrorText : '',
					},
					expiry_date : {
					cardExpiryErrorText : '',
					},
					cvc : {
					cardCvcErrorText : 'invalid cvc number',
					},
					errorText : 'Invalid card details',
					};
    var create_element_obj = { callBack : 'createElements', customStyle : styleObj, customText : textObj}
    iframe.postMessage( JSON.stringify(create_element_obj), targetOrigin );
    
        jQuery('#edit-continue').click(function(e) {
            /* Fetch the payment name from the payment selection */
            var payment = (jQuery('input[name="panes[payment][payment_method]"]:checked').length) ? jQuery('input[name="panes[payment][payment_method]"]:checked').val() : jQuery('#nn_admin_payment').val();
            if (payment == 'novalnet_cc') {
                    if((jQuery('#nn_cc_hash').val() == '') && jQuery('#load_cc_iframe').css('display') == 'block'){
                    e.preventDefault();
                    iframe.postMessage( { callBack : 'getHash' }, targetOrigin );
                }
            }
        });
        
        jQuery('#edit-submit').click(function(e) {
            /* Fetch the payment name from the payment selection */
            var payment = (jQuery('input[name="panes[payment][payment_method]"]:checked').length) ? jQuery('input[name="panes[payment][payment_method]"]:checked').val() : jQuery('#nn_admin_payment').val();
            if (payment == 'novalnet_cc') {
                    if((jQuery('#nn_cc_hash').val() == '') && jQuery('#load_cc_iframe').css('display') == 'block'){
                    e.preventDefault();
                    var messageObj = { callBack : 'getHash' }
                    iframe.postMessage( messageObj, targetOrigin );
                }
            }
        });
}

if ( window.addEventListener ) {
    // addEventListener works for all major browsers
    window.addEventListener('message', function(e) {
        addEvent(e);
    }, false);
} else {
    // attachEvent works for IE8
    window.attachEvent('onmessage', function (e) {
        addEvent(e);
    });
}

// Function to handle Event Listener
function addEvent(e) {
    if ( e.origin === 'https://secure.novalnet.de') {
		// Convert message string to object - eval
		var data = (typeof e.data === 'string' ) ? eval('(' + e.data.replace(/(<([^>]+)>)/gi, "") + ')') : e.data;
        if (data['callBack'] == 'getHash') {
            if (data['result'] == 'success') {
                jQuery('#novalnet_cc_uniqueid').val(data['unique_id']);
                jQuery('#nn_cc_hash').val(data['hash']);
                jQuery('#nn_cc_hash_generated').val(1)
                if(jQuery('#nn_admin_payment').val() && jQuery('#nn_cc_admin').val() == 1) {
                    jQuery('#edit-submit').click();
                }else {
                    jQuery('#edit-continue').click();
                }
            } else {
                alert(jQuery('<textarea />').html(data['error_message']).text());
                e.stopImmediatePropagation();
            }
        } else if (data['callBack'] == 'getHeight') {
            document.getElementById('nnIframe').height = data['contentHeight'];
        }
    }
}

function ccloadiframe(request){
    var targetOrgin = 'https://secure.novalnet.de';
    var iframe = jQuery('#nnIframe')[0];
    iframe = iframe.contentWindow ? iframe.contentWindow : iframe.contentDocument.defaultView;
    iframe.postMessage( request, targetOrgin );
} // CC end
