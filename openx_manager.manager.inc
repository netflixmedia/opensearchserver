<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Managers.
 */

/**
 * Implementation of hook_access().
 *
 * For Managers
 */
function _openx_node_manager_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create openx managers', $account);
  }

  if ($op == 'update' or $op == 'delete') {
    if (user_access('edit own openx managers', $account) and ($account->uid == $node->uid)) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_delete().
 *
 * For Managers
 */
function _openx_node_manager_delete(&$node) {
  _openx_manager_api('deleteManager', (int) $node->aid);
  db_query('DELETE FROM {openx_manager_manager} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Managers
 */
function _openx_node_manager_form(&$node, $form_state) {
  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Manager'),
    '#required'       => TRUE,
    '#default_value'  => $node->title,
    '#weight'         => -5,
  );
  $form['mid'] = array(
    '#value'          => $node->mid,
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

  $form['#submit'][] = '_openx_node_manager_submit';
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Managers
 */
function _openx_node_manager_insert(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_manager', $node->openx_node_fields);
}

/**
 * Implementation of hook_load().
 *
 * For Managers
 */
function _openx_node_manager_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_manager} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_manager_submit(&$form, &$form_state) {
  if ($form_state['values']['op'] != t('Preview')) {
    // Increase the code readability
    $node = &$form_state['values'];

    $data = array(
      'agencyName'      => $node['title'],
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
      $node['mid'] = _openx_manager_api('addAgency', $data);
    }
    else {
      $data['managerId'] = (int) $node['mid'];
      _openx_manager_api('modifyAgency', $data);
    }

    $node['openx_node_fields'] = array(
      'mid'                   => $node['mid'],
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
 * For Managers
 */
function _openx_node_manager_update(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_manager', $node->openx_node_fields, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Managers
 */
function _openx_node_manager_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }
}

/**
 * Implementation of hook_load().
 *
 * For Managers
 */
function _openx_node_manager_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->content['manager'] = array(
    '#value'  => theme('manager', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Managers fields
 */
function theme_manager(&$node) {
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
