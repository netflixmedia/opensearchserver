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
    $advertiser = node_load($_GET['nid']);
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
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Campaigns
 */
function _openx_node_campaign_insert(&$node) {
  $node->contract_activation = mktime(0, 0, 0, $node->contract_activation['month'],
      $node->contract_activation['day'], $node->contract_activation['year']);
  $node->contract_expiration = mktime(0, 0, 0, $node->contract_expiration['month'],
      $node->contract_expiration['day'], $node->contract_expiration['year']);

  // Required and optional values on OpenX server
  $data = array(
    'campaignName'  => $node->title,
    'advertiserId'  => (int) $node->aid,
  );
  if (!empty($node->contract_activation)) {
    $data['startDate'] = $node->contract_activation;
  }
  if (!empty($node->contract_expiration)) {
    $data['endDate'] = $node->contract_expiration;
  }
  if (!empty($node->inventory_impressions)) {
    $data['impressions'] = (int) $node->inventory_impressions;
  }
  if (!empty($node->inventory_clicks)) {
    $data['clicks'] = (int) $node->inventory_clicks;
  }
  if (!empty($node->contract_expiration)) {
    $data['priority'] = (int) $node->contract_expiration;
  }
  if (!empty($node->distribution_weight)) {
    $data['weight'] = (int) $node->distribution_weight;
  }
  $node->cid = _openx_manager_api('addCampaign', $data);

  db_query("INSERT INTO {openx_manager_campaign}
    (nid, cid, aid, inventory_impressions, inventory_clicks,
    inventory_conversions, contract_activation, contract_expiration,
    contract_revenue, contract_revenue_unit, priority_level_level,
    priority_distribution_level, priority_misc_hide, priority_misc_positioning,
    delivery_limit_total, delivery_limit_session, delivery_reset_view)
    VALUES (%d, %d, %d, %d, %d,
    %d, %d, %d,
    %d, %d, %d,
    %d, %d, %d,
    %d, %d, %d)",
    $node->nid, $node->cid, $node->aid, $node->inventory_impressions, $node->inventory_clicks,
    $node->inventory_conversions, $node->contract_activation, $node->contract_expiration,
    $node->contract_revenue, $node->contract_revenue_unit, $node->priority_level_level,
    $node->priority_distribution_level, $node->priority_misc_hide, $node->priority_misc_positioning,
    $node->delivery_limit_total, $node->delivery_limit_session, $node->delivery_reset_view
  );
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
 * Implementation of hook_update().
 *
 * For Campaigns
 */
function _openx_node_campaign_update(&$node) {
  // Required and optional values on OpenX server
  $data = array(
    'campaignId'    => (int) $node->cid,
    'campaignName'  => $node->title,
    'advertiserId'  => (int) $node->aid,
  );
//   if (!empty($node->contract_activation)) {
//     $data['startDate'] = $node->contract_activation;
//   }
//   if (!empty($node->contract_expiration)) {
//     $data['endDate'] = $node->contract_expiration;
//   }
  if (!empty($node->inventory_impressions)) {
    $data['impressions'] = (int) $node->inventory_impressions;
  }
  if (!empty($node->inventory_clicks)) {
    $data['clicks'] = (int) $node->inventory_clicks;
  }
  if (!empty($node->priority_level_level)) {
    $data['priority'] = (int) $node->priority_level_level;
  }
  if (!empty($node->distribution_weight)) {
    $data['weight'] = (int) $node->distribution_weight;
  }
  _openx_manager_api('modifyCampaign', $data);

  $node->contract_activation = mktime(0, 0, 0, $node->contract_activation['month'],
      $node->contract_activation['day'], $node->contract_activation['year']);
  $node->contract_expiration = mktime(0, 0, 0, $node->contract_expiration['month'],
      $node->contract_expiration['day'], $node->contract_expiration['year']);
  db_query("UPDATE {openx_manager_campaign} SET
    cid = %d, aid = %d, inventory_impressions = %d, inventory_clicks = %d,
    inventory_conversions = %d, contract_activation = %d, contract_expiration = %d,
    contract_revenue = %d, contract_revenue_unit = %d, priority_level_level = %d,
    priority_distribution_level = %d, priority_misc_hide = %d, priority_misc_positioning = %d,
    delivery_limit_total = %d, delivery_limit_session = %d, delivery_reset_view = %d
    WHERE nid = %d",
    $node->cid, $node->aid, $node->inventory_impressions, $node->inventory_clicks,
    $node->inventory_conversions, $node->contract_activation, $node->contract_expiration,
    $node->contract_revenue, $node->contract_revenue_unit, $node->priority_level_level,
    $node->priority_distribution_level, $node->priority_misc_hide, $node->priority_misc_positioning,
    $node->delivery_limit_total, $node->delivery_limit_session, $node->delivery_reset_view,
    $node->nid
  );
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
