<?php
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Views integration.
 */

/**
 * Implementation of hook_default_view_views().
 */
function openx_manager_views_default_views() {
  $views = array();
  _openx_manager_default_views_banner($views);
  return $views;
}

/**
 * Create the banner list.
 */
function _openx_manager_default_views_banner(&$views) {
  $view = new view;
  $view->name = 'openx_manager_banners';
  $view->description = 'OpenX Manager - Banner';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Title',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'destination_url' => array(
      'label' => 'Destination URL',
      'exclude' => 0,
      'id' => 'destination_url',
      'table' => 'openx_manager_banner',
      'field' => 'destination_url',
      'relationship' => 'none',
    ),
    'btype' => array(
      'label' => 'Type',
      'exclude' => 0,
      'id' => 'btype',
      'table' => 'openx_manager_banner',
      'field' => 'btype',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'ocid' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'break_phrase' => 0,
      'not' => 0,
      'id' => 'ocid',
      'table' => 'openx_manager_banner',
      'field' => 'ocid',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'openx_advertiser' => 0,
        'openx_banner' => 0,
        'openx_campaign' => 0,
        'openx_manager' => 0,
        'openx_publisher' => 0,
        'openx_user' => 0,
        'openx_zone' => 0,
        'page' => 0,
        'story' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(
        '1' => 0,
      ),
      'validate_argument_type' => 'tid',
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'openx_banner' => 'openx_banner',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view openx campaigns',
  ));
  $handler->override_option('empty', 'None created.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_ajax', TRUE);
  $handler->override_option('use_pager', 'mini');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'contract_expiration' => 'contract_expiration',
      'inventory_clicks' => 'inventory_clicks',
      'inventory_conversions' => 'inventory_conversions',
      'inventory_impressions' => 'inventory_impressions',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'contract_expiration' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'inventory_clicks' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'inventory_conversions' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'inventory_impressions' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $views[$view->name] = $view;
}
