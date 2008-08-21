<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Campaigns.
 */

// OpenX constants (on constants.php)
define('MAX_FINANCE_CPM',    1);
define('MAX_FINANCE_CPC',    2);
define('MAX_FINANCE_CPA',    3);
define('MAX_FINANCE_MT',     4); // Monthly Tennancy
define('MAX_FINANCE_RS',     5); // % Revenue split     (zone-only)
define('MAX_FINANCE_BV',     6); // % Basket value      (zone-only)
define('MAX_FINANCE_AI',     7); // Amount per item     (zone-only)
define('MAX_FINANCE_ANYVAR', 8); // % of any variable   (zone-only)
define('MAX_FINANCE_VARSUM', 9); // % of a variable sum (zone-only)

/**
 * Implementation of hook_delete().
 *
 * For Campaigns
 */
function _openx_node_campaign_delete(&$node) {
  _openx_manager_api('deleteCampaign', (int) $node->cid);
  db_query('DELETE FROM {openx_manager_campaign} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Campaigns
 */
function _openx_node_campaign_form(&$node, $form_state) {
  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Campaign'),
    '#required'       => TRUE,
    '#default_value'  => $node->title,
    '#weight'         => -5,
  );
  // Internal values
  $form['cid'] = array(
    '#value'          => $node->cid,
    '#type'           => 'value',
  );
  if (empty($node->aid)) {
    $aid = $_GET['aid'];
    $advertiser = node_load($_GET['nid']);
  }
  else {
    $aid = $node->aid;
    $advertiser = node_load(db_result(db_query('SELECT nid FROM {openx_manager_advertiser}
      WHERE aid = %d', $aid)));
  }
  $form['aid'] = array(
    '#value'          => $aid,
    '#type'           => 'value',
  );
  $form['advertiser'] = array(
    '#value'          => l($advertiser->title, 'node/'. $advertiser->nid),
    '#title'          => t('Advertiser'),
    '#type'           => 'item',
  );

  $form['inventory'] = array(
    '#description'    => t('-1 for unlimeted'),
    '#title'          => t('Inventory'),
    '#type'           => 'fieldset',
  );
  $form['inventory']['inventory_impressions'] = array(
    '#default_value'  => empty($node->inventory_impressions) ? -1 : $node->inventory_impressions,
    '#title'          => t('Impressions booked'),
    '#type'           => 'textfield',
  );
  $form['inventory']['inventory_clicks'] = array(
    '#default_value'  => empty($node->inventory_clicks) ? -1 : $node->inventory_clicks,
    '#title'          => t('Clicks booked'),
    '#type'           => 'textfield',
  );
  $form['inventory']['inventory_conversions'] = array(
    '#default_value'  => empty($node->inventory_conversions) ? -1 : $node->inventory_conversions,
    '#title'          => t('Conversions booked'),
    '#type'           => 'textfield',
  );
  $form['contract'] = array(
    '#title'          => t('Contract'),
    '#type'           => 'fieldset',
  );
  if (is_numeric($node->contract_activation)) {
    $node->contract_activation = array(
      'day'   => date('j', $node->contract_activation),
      'month' => date('n', $node->contract_activation),
      'year'  => date('Y', $node->contract_activation),
    );
  }
  $form['contract']['contract_activation'] = array(
    '#default_value'  => $node->contract_activation,
    '#title'          => t('Activation date'),
    '#type'           => 'date',
  );
  if (is_numeric($node->contract_expiration)) {
    $node->contract_expiration = array(
      'day'   => date('j', $node->contract_expiration),
      'month' => date('n', $node->contract_expiration),
      'year'  => date('Y', $node->contract_expiration),
    );
  }
  $form['contract']['contract_expiration'] = array(
    '#default_value'  => $node->contract_expiration,
    '#title'          => t('Expiration date'),
    '#type'           => 'date',
  );
  $form['contract']['contract_revenue'] = array(
    '#default_value'  => $node->contract_revenue,
    '#title'          => t('Revenue Information'),
    '#type'           => 'textfield',
  );
  $form['contract']['contract_revenue_unit'] = array(
    '#default_value'  => empty($node->contract_revenue_unit) ? MAX_FINANCE_CPM : $node->contract_revenue_unit,
    '#options'        => array(
      MAX_FINANCE_CPM   => t('CPM'),
      MAX_FINANCE_CPC   => t('CPC'),
      MAX_FINANCE_CPA   => t('CPA'),
      MAX_FINANCE_MT    => t('Monthly Tenancy'),
    ),
    '#type'           => 'select',
  );
  $form['priority'] = array(
    '#title'          => t('Priority'),
    '#type'           => 'fieldset',
  );
  $form['priority']['level'] = array(
    '#title'          => t('Priority Level'),
    '#type'           => 'fieldset',
  );
  $form['priority']['level']['priority_level_level'] = array(
    '#default_value'  => $node->priority_level_level,
    '#options'        => array(
      -1  => t('Exclusive (Overrides other linked campaigns)'),
      5   => t('High (Paid campaigns)'),
      0   => t('Low (House and unpaid campaigns)'),
    ),
    '#type'           => 'radios',
  );
  $form['priority']['distribution'] = array(
    '#title'          => t('Distribution'),
    '#type'           => 'fieldset',
  );
  $form['priority']['distribution']['priority_distribution_level'] = array(
    '#default_value'  => $node->priority_distribution_level,
    '#options'        => array(
      t('Automatic - Distribute the remaining inventory evenly over the remaining number of days'),
      t('Manual'),
      t('None - Set the campaign weight to'),
    ),
    '#type'           => 'radios',
  );
  $form['priority']['misc'] = array(
    '#title'          => t('Miscellaneous'),
    '#type'           => 'fieldset',
  );
  $form['priority']['misc']['priority_misc_hide'] = array(
    '#default_value'  => $node->priority_misc_hide,
    '#title'          => t('Hide the advertiser and websites of this campaign'),
    '#type'           => 'checkbox',
  );
  $form['priority']['misc']['priority_misc_positioning'] = array(
    '#default_value'  => $node->priority_misc_positioning,
    '#title'          => t('Companion positioning'),
    '#type'           => 'checkbox',
  );
  $form['delivery'] = array(
    '#title'          => t('Delivery'),
    '#type'           => 'fieldset',
  );
  $form['delivery']['delivery_limit_total'] = array(
    '#default_value'  => $node->delivery_limit_total,
    '#title'          => t('Limit campaign views in total'),
    '#type'           => 'textfield',
  );
  $form['delivery']['delivery_limit_session'] = array(
    '#default_value'  => $node->delivery_limit_session,
    '#title'          => t('Limit campaign views per session'),
    '#type'           => 'textfield',
  );
  $form['delivery']['delivery_reset_view'] = array(
    '#default_value'  => $node->delivery_reset_view,
    '#title'          => t('Reset view counters after'),
    '#type'           => 'date',
  );


  $form['#submit'][] = '_openx_node_campaign_submit';
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Campaigns
 */
function _openx_node_campaign_insert(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_campaign', $node->openx_node_fields);
}

/**
 * Implementation of hook_load().
 *
 * For Campaigns
 */
function _openx_node_campaign_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_campaign} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_campaign_submit(&$form, &$form_state) {
  $node = &$form_state['values'];

  $node['contract_activation'] = mktime(0, 0, 0, $node['contract_activation']['month'],
      $node['contract_activation']['day'], $node['contract_activation']['year']);
  $node['contract_expiration'] = mktime(0, 0, 0, $node['contract_expiration']['month'],
      $node['contract_expiration']['day'], $node['contract_expiration']['year']);

  // Required and optional values on OpenX server
  $data = array(
    'campaignName'  => $node['title'],
    'advertiserId'  => (int) $node['aid'],
  );
  if (!empty($node['contract_activation'])) {
    include_once './includes/xmlrpc.inc';
    $data['startDate'] = xmlrpc_date($node['contract_activation']);
  }
  if (!empty($node['contract_expiration'])) {
    include_once './includes/xmlrpc.inc';
    $data['endDate'] = xmlrpc_date($node['contract_expiration']);
  }
  if (!empty($node['inventory_impressions'])) {
    $data['impressions'] = (int) $node['inventory_impressions'];
  }
  if (!empty($node['inventory_clicks'])) {
    $data['clicks'] = (int) $node['inventory_clicks'];
  }
  if (!empty($node['contract_expiration'])) {
    $data['priority'] = (int) $node['contract_expiration'];
  }
  if (!empty($node['distribution_weight'])) {
    $data['weight'] = (int) $node['distribution_weight'];
  }

  // Update or insert
  if (empty($node['nid'])) {
    $node['cid'] = _openx_manager_api('addCampaign', $data);
  }
  else {
    $data['campaignId'] = (int) $node['cid'];
    _openx_manager_api('modifyCampaign', $data);
  }

  $node['openx_node_fields'] = array(
    'cid'                         => $node['cid'],
    'aid'                         => $node['aid'],
    'inventory_impressions'       => $node['inventory_impressions'],
    'inventory_clicks'            => $node['inventory_clicks'],
    'inventory_conversions'       => $node['inventory_conversions'],
    'contract_activation'         => $node['contract_activation'],
    'contract_expiration'         => $node['contract_expiration'],
    'contract_revenue'            => $node['contract_revenue'],
    'contract_revenue_unit'       => $node['contract_revenue_unit'],
    'priority_level_level'        => $node['priority_level_level'],
    'priority_distribution_level' => $node['priority_distribution_level'],
    'priority_misc_hide'          => $node['priority_misc_hide'],
    'priority_misc_positioning'   => $node['priority_misc_positioning'],
    'delivery_limit_total'        => $node['delivery_limit_total'],
    'delivery_limit_session'      => $node['delivery_limit_session'],
    'delivery_reset_view'         => $node['delivery_reset_view'],
  );
}

/**
 * Implementation of hook_update().
 *
 * For Campaigns
 */
function _openx_node_campaign_update(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_campaign', $node->openx_node_fields, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Campaigns
 */
function _openx_node_campaign_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }

  // Check if the integer values are really integers
  $integers_values = array(
    'inventory_impressions',
    'inventory_clicks',
    'inventory_conversions',
    'contract_revenue',
    'priority_level_level',
    'priority_distribution_level',
    'priority_misc_hide',
    'priority_misc_positioning',
    'delivery_limit_total',
    'delivery_limit_session',
  );
  foreach ($integers_values as $value) {
    if (!empty($node->{$value}) and !is_numeric($node->{$value})) {
      form_set_error($value, t('It must be numeric'));
    }
  }
}

/**
 * Implementation of hook_load().
 *
 * For Campaigns
 */
function _openx_node_campaign_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->advertiser = node_load(db_result(db_query('SELECT nid FROM {openx_manager_advertiser}
    WHERE aid = %d', $node->aid)));
  $node->content['campaign'] = array(
    '#value'  => theme('campaign', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Campaign fields
 */
function theme_campaign(&$node) {
  $fields['advertiser'] = array(
    '#value'          => l($node->advertiser->title, 'node/'. $node->advertiser->nid),
    '#title'          => t('Advertiser'),
    '#type'           => 'item',
  );
  $fields['inventory'] = array(
    '#title'          => t('Inventory'),
    '#type'           => 'fieldset',
    '#weight'         => 1,
  );
  $fields['inventory']['inventory_impressions'] = array(
    '#value'          => ($node->inventory_impressions == -1) ? 'unlimited' : $node->inventory_impressions,
    '#title'          => t('Impressions booked'),
    '#type'           => 'item',
  );
  $fields['inventory']['inventory_clicks'] = array(
    '#value'          => ($node->inventory_clicks == -1) ? 'unlimited' : $node->inventory_clicks,
    '#title'          => t('Clicks booked'),
    '#type'           => 'item',
  );
  $fields['inventory']['inventory_conversions'] = array(
    '#value'          => ($node->inventory_conversions == -1) ? 'unlimited' : $node->inventory_conversions,
    '#title'          => t('Conversions booked'),
    '#type'           => 'item',
  );
  $fields['contract'] = array(
    '#title'          => t('Contract'),
    '#type'           => 'fieldset',
    '#weight'         => 2,
  );
  $fields['contract']['contract_activation'] = array(
    '#value'          => format_date($node->contract_activation, 'small'),
    '#title'          => t('Activation date'),
    '#type'           => 'item',
  );
  $fields['contract']['contract_expiration'] = array(
    '#value'          => format_date($node->contract_expiration, 'small'),
    '#title'          => t('Expiration date'),
    '#type'           => 'item',
  );
  $options = array(
    MAX_FINANCE_CPM   => t('CPM'),
    MAX_FINANCE_CPC   => t('CPC'),
    MAX_FINANCE_CPA   => t('CPA'),
    MAX_FINANCE_MT    => t('Monthly Tenancy'),
  );
  $fields['contract']['contract_revenue'] = array(
    '#value'          => check_plain($node->contract_revenue) .' '. $options[$node->contract_revenue_unit],
    '#title'          => t('Revenue Information'),
    '#type'           => 'item',
  );
  $fields['priority'] = array(
    '#title'          => t('Priority'),
    '#type'           => 'fieldset',
    '#weight'         => 3,
  );
  $options = array(
    -1  => t('Exclusive (Overrides other linked campaigns)'),
    5   => t('High (Paid campaigns)'),
    0   => t('Low (House and unpaid campaigns)'),
  );
  $fields['priority']['priority_level_level'] = array(
    '#value'          => $options[$node->priority_level_level],
    '#title'          => t('Priority'),
    '#type'           => 'item',
  );
  $options = array(
    t('Automatic - Distribute the remaining inventory evenly over the remaining number of days'),
    t('Manual'),
    t('None - Set the campaign weight to'),
  );
  $fields['priority']['priority_distribution_level'] = array(
    '#value'          => $options[$node->priority_distribution_level],
    '#title'          => t('Distribution'),
    '#type'           => 'item',
  );
  $fields['priority']['priority_misc_hide'] = array(
    '#value'          => $node->priority_misc_hide,
    '#title'          => t('Hide the advertiser and websites of this campaign'),
    '#type'           => 'item',
  );
  $fields['priority']['priority_misc_positioning'] = array(
    '#value'          => $node->priority_misc_positioning,
    '#title'          => t('Companion positioning'),
    '#type'           => 'item',
  );
  $fields['delivery'] = array(
    '#title'          => t('Delivery'),
    '#type'           => 'fieldset',
    '#weight'         => 4,
  );
  $fields['delivery']['delivery_limit_total'] = array(
    '#value'          => $node->delivery_limit_total,
    '#title'          => t('Limit campaign views in total'),
    '#type'           => 'item',
  );
  $fields['delivery']['delivery_limit_session'] = array(
    '#value'          => $node->delivery_limit_session,
    '#title'          => t('Limit campaign views per session'),
    '#type'           => 'item',
  );
  $fields['delivery']['delivery_reset_view'] = array(
    '#value'          => $node->delivery_reset_view,
    '#title'          => t('Reset view counters after'),
    '#type'           => 'item',
  );

  // Print all field in the right order
  $output = '';
  foreach ($fields as $field) {
    $output .= drupal_render($field);
  }
  return $output;
}
