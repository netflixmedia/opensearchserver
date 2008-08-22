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
  $form['ouid'] = array(
    '#value'          => $node->ouid,
    '#type'           => 'value',
  );
  $form['contact'] = array(
    '#default_value'  => $node->contact,
    '#required'       => TRUE,
    '#title'          => t('Contact name'),
    '#type'           => 'textfield',
  );
  $form['email'] = array(
    '#default_value'  => $node->email,
    '#title'          => t('Email'),
    '#type'           => 'textfield',
  );
  $form['username'] = array(
    '#default_value'  => $node->username,
    '#title'          => t('Username'),
    '#type'           => 'textfield',
  );
  $form['password'] = array(
    '#default_value'  => $node->password,
    '#title'          => t('Password'),
    '#type'           => 'password',
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
  if ($form_state['values']['op'] != t('Preview')) {
    // Increase the code readability
    $node = &$form_state['values'];

    $data = array(
      'userName'        => $node['title'],
      'contactName'     => $node['contact'],
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
      $data['userId'] = (int) $node['ouid'];
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
  $node->content['ouser'] = array(
    '#value'  => theme('ouser', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the User fields
 */
function theme_ouser(&$node) {
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
