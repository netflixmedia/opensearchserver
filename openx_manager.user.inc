<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Users.
 */

/**
 * Implementation of hook_access().
 *
 * For Users
 */
function _openx_node_user_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create openx users', $account);
  }

  if ($op == 'update' or $op == 'delete') {
    if (user_access('edit own openx users', $account) and ($account->uid == $node->uid)) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_delete().
 *
 * For Users
 */
function _openx_node_user_delete(&$node) {
  _openx_manager_api('deleteUser', (int) $node->cid);
  db_query('DELETE FROM {openx_manager_user} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Users
 */
function _openx_node_user_form(&$node, $form_state) {
  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('User'),
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
      -1  => t('Exclusive (Overrides other linked users)'),
      5   => t('High (Paid users)'),
      0   => t('Low (House and unpaid users)'),
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
      t('None - Set the user weight to'),
    ),
    '#type'           => 'radios',
  );
  $form['priority']['misc'] = array(
    '#title'          => t('Miscellaneous'),
    '#type'           => 'fieldset',
  );
  $form['priority']['misc']['priority_misc_hide'] = array(
    '#default_value'  => $node->priority_misc_hide,
    '#title'          => t('Hide the advertiser and websites of this user'),
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
    '#title'          => t('Limit user views in total'),
    '#type'           => 'textfield',
  );
  $form['delivery']['delivery_limit_session'] = array(
    '#default_value'  => $node->delivery_limit_session,
    '#title'          => t('Limit user views per session'),
    '#type'           => 'textfield',
  );
  $form['delivery']['delivery_reset_view'] = array(
    '#default_value'  => $node->delivery_reset_view,
    '#title'          => t('Reset view counters after'),
    '#type'           => 'date',
  );

  $form['#submit'][] = '_openx_node_user_submit';
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Users
 */
function _openx_node_user_insert(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_user', $node->openx_node_fields);
}

/**
 * Implementation of hook_load().
 *
 * For Users
 */
function _openx_node_user_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_user} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_user_submit(&$form, &$form_state) {
  // Increase the code readability
  $node = &$form_state['values'];

  $data = array(
    'userName'        => $node['title'],
    'contactName'     => $node['info_contact'],
  );
  if (!empty($node['email'])) {
    $data['emailAddress'] = $node['email'];
  }
  if (!empty($node['username'])) {
    $data['username'] = $node['username'];
  }
  if (!empty($node['password'])) {
    $data['password'] = $node['password'];
  }

  // Update or insert
  if (empty($node['nid'])) {
    $node['ouid'] = _openx_manager_api('addUser', $data);
  }
  else {
    $data['userId'] = (int) $node['mid'];
    _openx_manager_api('modifyAgency', $data);
  }

  $node['openx_node_fields'] = array(
    'ouid'                  => $node['ouid'],
    'contact'               => $node['contact'],
    'email'                 => $node['email'],
    'username'              => $node['username'],
    'password'              => $node['password'],
  );
}

/**
 * Implementation of hook_update().
 *
 * For Users
 */
function _openx_node_user_update(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_user', $node->openx_node_fields, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Users
 */
function _openx_node_user_validate(&$node) {
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
 * For Users
 */
function _openx_node_user_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->advertiser = node_load(db_result(db_query('SELECT nid FROM {openx_manager_advertiser}
    WHERE aid = %d', $node->aid)));
  $node->content['user'] = array(
    '#value'  => theme('user', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the User fields
 */
function theme_user(&$node) {
  $fields['contact'] = array(
    '#value'          => $node->contact,
    '#title'          => t('Contact name'),
    '#type'           => 'item',
  );
  $fields['email'] = array(
    '#value'          => $node->email,
    '#title'          => t('Email'),
    '#type'           => 'item',
  );

  // Print all field in the right order
  $output = '';
  foreach ($fields as $field) {
    $output .= drupal_render($field);
  }
  return $output;
}
