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
 * Script   : uc_novalnet.views_default.inc
 *
 */

/**
 *
 * Implements hook__views_default_views().
 *
 * @param none
 * @return array $views
 */
function uc_novalnet_views_default_views() {
    $view = new view;
    $view->name = 'uc_novalnet';
    $view->description = '';
    $view->tag = 'Ubercart Novalnet Orders';
    $view->base_table = 'uc_orders';
    $view->human_name = 'uc_novalnet_order';
    $view->core = 0;
    $view->api_version = '3.0';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

    /* Display: Defaults */
    $handler = $view->new_display('default', 'Defaults', 'default');

    $handler->display->display_options['title'] = 'Novalnet Orders';
    $handler->display->display_options['access']['type'] = 'none';
    $handler->display->display_options['cache']['type'] = 'none';
    $handler->display->display_options['query']['type'] = 'views_query';
    $handler->display->display_options['query']['options']['query_comment'] = FALSE;
    $handler->display->display_options['exposed_form']['type'] = 'basic';
    $handler->display->display_options['pager']['type'] = 'full';
    $handler_options = array('items_per_page' => '30', 'items_per_page' => '0', 'items_per_page' => '0');
    foreach ( $handler_options as $handlers_key => $handlers_val ) {
      $handler->display->display_options['pager']['options'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['style_plugin'] = 'table';
    $handler_style_options = array(
        'columns' => array(
            'actions' => 'actions',
            'order_id' => 'order_id',
            'billing_full_name' => 'billing_full_name',
            'order_status' => 'order_status',
            'created' => 'created',
            'order_total' => 'order_total',
        ),
       'default' => 'order_id',
       'info' => array(
      'actions' => array(
        'align' => '',
        'separator' => '',
      ),
      'order_id' => array(
        'sortable' => 1,
        'align' => '',
        'separator' => '',
      ),
      'billing_full_name' => array(
        'align' => '',
        'separator' => '',
      ),
      'order_status' => array(
        'sortable' => 1,
        'align' => '',
        'separator' => '',
      ),
      'created' => array(
        'sortable' => 1,
        'align' => '',
        'separator' => '',
      ),
      'order_total' => array(
        'sortable' => 1,
        'align' => '',
        'separator' => '',
      ),
    ),
    'sticky' => TRUE,
    'order' => 'desc',
    );
    foreach ( $handler_style_options as $handlers_key => $handlers_val ) {
      $handler->display->display_options['style_options'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Actions */
    $handler_fields_actions = array('id' => 'actions', 'table' => 'uc_orders', 'field' => 'actions');
    foreach ( $handler_fields_actions as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['actions'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Order ID */
    $handler_fields_order_id = array('id' => 'order_id', 'table' => 'uc_orders', 'field' => 'order_id', 'link_to_order' => 1);
    foreach ( $handler_fields_order_id as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_id'][$handlers_key] = $handlers_val;
    }
    /* Field: User: Uid */
    $handler_fields_uid = array('id' => 'uid', 'table' => 'users', 'field' => 'uid', 'label' => '', 'exclude' => TRUE, 'element_label_colon' => FALSE, 'link_to_user' => FALSE);
    foreach ( $handler_fields_uid as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['uid'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['fields']['uid']['alter']['text'] = '[billing_full_name]';
    $handler->display->display_options['fields']['uid']['alter']['path'] = '[uid]';
    /* Field: Order: Billing address: Full name */
    $handler_fields_billing_full_name = array('id' => 'billing_full_name', 'table' => 'uc_orders', 'field' => 'billing_full_name', 'label' => 'Customer');
    foreach ( $handler_fields_billing_full_name as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['billing_full_name'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['fields']['billing_full_name']['alter']['make_link'] = TRUE;
    $handler->display->display_options['fields']['billing_full_name']['alter']['path'] = 'user/[uid]';
    /* Field: Order: Order total */
    $handler_fields_order_total = array('id' => 'order_total', 'table' => 'uc_orders', 'field' => 'order_total', 'label' => 'Total', 'hide_alter_empty' => FALSE, 'precision' => '0');
    foreach ( $handler_fields_order_total as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_total'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Creation date */
    $handler_fields_created = array('id' => 'created', 'table' => 'uc_orders', 'field' => 'created', 'label' => 'Purchase date', 'hide_alter_empty' => FALSE, 'date_format' => 'uc_store');
    foreach ( $handler_fields_created as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['created'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Order status */
    $handler_fields_order_status = array('id' => 'order_status', 'table' => 'uc_orders', 'field' => 'order_status', 'label' => 'Status', 'hide_alter_empty' => FALSE);
    foreach ( $handler_fields_order_status as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_status'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Payment method */
    $handler_fields_payment_method = array('id' => 'payment_method', 'table' => 'uc_orders', 'field' => 'payment_method', 'label' => 'Payment Method');
    foreach ( $handler_fields_payment_method as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['payment_method'][$handlers_key] = $handlers_val;
    }
    /* Filter criterion: Order: Order ID */
    $handler_filters_order_id = array('id' => 'order_id', 'table' => 'uc_orders', 'field' => 'order_id', 'group' => 0, 'exposed' => TRUE);
    foreach ( $handler_filters_order_id as $handlers_key => $handlers_val ) {
      $handler->display->display_options['filters']['order_id'][$handlers_key] = $handlers_val;
    }
    $handlers_expose = array('operator_id' => 'order_id_op', 'label' => 'View order number', 'operator' => 'order_id_op', 'identifier' => 'order_id');
    foreach ( $handlers_expose as $handlers_key => $handlers_val ) {
      $handler->display->display_options['filters']['order_id']['expose'][$handlers_key] = $handlers_val;
    }
    /* Filter criterion: Order: Order status */
    $handlers_status = array('id' => 'order_status', 'table' => 'uc_orders', 'field' => 'order_status', 'value' => array(
      '_active' => '_active',
      'abandoned' => 'abandoned',
      'canceled' => 'canceled',
      'pending' => 'pending',
      'processing' => 'processing',
      'payment_received' => 'payment_received',
      'completed' => 'completed',
    ),
    'group' => 0, 'exposed' => TRUE
    );
    foreach ( $handlers_status as $handlers_key => $handlers_val ) {
      $handler->display->display_options['filters']['order_status'][$handlers_key] = $handlers_val;
    }
    $handlers_order_status_expose = array('operator_id' => 'order_status_op', 'label' => 'View by status', 'operator' => 'order_status_op', 'identifier' => 'order_status', 'reduce' => 0);
    foreach ( $handlers_order_status_expose as $handlers_key => $handlers_val ) {
      $handler->display->display_options['filters']['order_status']['expose'][$handlers_key] = $handlers_val;
    }
    /* Filter criterion: Order: Payment method */
    $handler_payment_method_expose = array('id' => 'payment_method', 'table' => 'uc_orders', 'field' => 'payment_method', 'value' => array(
      'novalnet_giropay' => 'novalnet_giropay',
      'novalnet_prepayment' => 'novalnet_prepayment',
      'novalnet_eps' => 'novalnet_eps',
      'novalnet_ideal' => 'novalnet_ideal',
      'novalnet_cc' => 'novalnet_cc',
      'novalnet_sepa' => 'novalnet_sepa',
      'novalnet_invoice' => 'novalnet_invoice',
      'novalnet_paypal' => 'novalnet_paypal',
      'novalnet_instantbank' => 'novalnet_instantbank',
      'novalnet_przelewy24' => 'novalnet_przelewy24',
      'novalnet_cashpayment' => 'novalnet_cashpayment',
    ));
    foreach ( $handler_payment_method_expose as $handlers_key => $handlers_val ) {
      $handler->display->display_options['filters']['payment_method'][$handlers_key] = $handlers_val;
    }

    /* Display: Admin page */
    $handler = $view->new_display('page', 'Admin page', 'admin_page');
    $handler->display->display_options['defaults']['fields'] = FALSE;
    /* Field: Order: Actions */
    $handlers_actions = array('id' => 'actions', 'table' => 'uc_orders', 'field' => 'actions');
    foreach ( $handlers_actions as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['actions'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Order ID */
    $handlers_order_id = array('id' => 'order_id', 'table' => 'uc_orders', 'field' => 'order_id', 'link_to_order' => 1);
    foreach ( $handlers_order_id as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_id'][$handlers_key] = $handlers_val;
    }
    /* Field: User: Uid */
    $handlers_uid = array('id' => 'uid', 'table' => 'users', 'field' => 'uid', 'label' => '', 'exclude' => TRUE, 'element_label_colon' => FALSE, 'link_to_user' => FALSE);
    foreach ( $handlers_uid as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['uid'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['fields']['uid']['alter']['text'] = '[billing_full_name]';
    $handler->display->display_options['fields']['uid']['alter']['path'] = '[uid]';
    /* Field: Order: Billing address: Full name */
    $handlers_billing_full_name = array('id' => 'billing_full_name', 'table' => 'uc_orders', 'field' => 'billing_full_name', 'label' => 'Customer');
    foreach ( $handlers_billing_full_name as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['billing_full_name'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['fields']['billing_full_name']['alter']['make_link'] = TRUE;
    $handler->display->display_options['fields']['billing_full_name']['alter']['path'] = 'user/[uid]';
    /* Field: Order: Order total */
    $handlers_order = array('id' => 'order_total', 'table' => 'uc_orders', 'field' => 'order_total', 'label' => 'Total', 'hide_alter_empty' => FALSE, 'precision' => '0');
    foreach ( $handlers_order as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_total'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Creation date */
    $handlers_created = array('id' => 'created', 'table' => 'uc_orders', 'field' => 'created', 'label' => 'Purchase date', 'hide_alter_empty' => FALSE, 'date_format' => 'uc_store');
    foreach ( $handlers_created as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['created'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Order status */
    $handlers_order_status = array('id' => 'order_status', 'table' => 'uc_orders', 'field' => 'order_status', 'label' => 'Status', 'hide_alter_empty' => FALSE);
    foreach ( $handlers_order_status as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['order_status'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Payment method */
    $handlers_payment_method = array('id' => 'payment_method', 'table' => 'uc_orders', 'field' => 'payment_method', 'label' => 'Payment Method');
    foreach ( $handlers_payment_method as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['payment_method'][$handlers_key] = $handlers_val;
    }
    /* Field: Order: Actions */
    $handlers_actions_1 = array('id' => 'actions_1', 'table' => 'uc_orders', 'field' => 'actions', 'element_label_colon' => 1, 'element_default_classes' => 1, 'hide_empty' => 0, 'empty_zero' => 0, 'hide_alter_empty' => 1);
    foreach ( $handlers_actions_1 as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['actions_1'][$handlers_key] = $handlers_val;
    }
    $handlers_actions_1_alter = array('alter_text' => 1, 'text' => 'Logs', 'make_link' => 1, 'path' => 'admin/novalnet/novalnet_orders/[order_id]/novalnet_log', 'absolute' => 0, 'external' => 0, 'replace_spaces' => 0, 'trim_whitespace' => 0, 'nl2br' => 0, 'word_boundary' => 1, 'ellipsis' => 1, 'strip_tags' => 0, 'trim' => 0, 'html' => 0);
    foreach ( $handlers_actions_1_alter as $handlers_key => $handlers_val ) {
      $handler->display->display_options['fields']['actions_1']['alter'][$handlers_key] = $handlers_val;
    }
    $handler->display->display_options['path'] = 'admin/novalnet/novalnet_orders';
    $handler->display->display_options['menu']['type'] = 'normal';
    $handler->display->display_options['menu']['title'] = 'Novalnet orders';
    $handler->display->display_options['menu']['weight'] = '';
    $handler->display->display_options['menu']['name'] = 'management';
    $translatables['uc_novalnet'] = array(
      t('Defaults'),
      t('Novalnet Orders'),
      t('more'),
      t('Apply'),
      t('Reset'),
      t('Sort by'),
      t('Asc'),
      t('Desc'),
      t('Items per page'),
      t('- All -'),
      t('Offset'),
      t('Actions'),
      t('Order ID'),
      t('[billing_full_name]'),
      t('[uid]'),
      t('Customer'),
      t('user/[uid]'),
      t('Total'),
      t('.'),
      t(','),
      t('Purchase date'),
      t('Status'),
      t('Payment Method'),
      t('View order number'),
      t('View by status'),
      t('Admin page'),
      t('Logs'),
      t('admin/novalnet/novalnet_orders/[order_id]/novalnet_log'),
    );
    $views[$view->name] = $view;
    return $views;
}
