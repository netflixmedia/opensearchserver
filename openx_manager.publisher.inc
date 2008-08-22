<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Publishers.
 */

/**
 * Implementation of hook_access().
 *
 * For Publishers
 */
function _openx_node_publisher_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create openx publishers', $account);
  }

  if ($op == 'update' or $op == 'delete') {
    if (user_access('edit own openx publishers', $account) and ($account->uid == $node->uid)) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_delete().
 *
 * For Publishers
 */
function _openx_node_publisher_delete(&$node) {
  _openx_manager_api('deletePublisher', (int) $node->cid);
  db_query('DELETE FROM {openx_manager_publisher} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Publishers
 */
function _openx_node_publisher_form(&$node, $form_state) {
dpm($node);
  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Publisher'),
    '#required'       => TRUE,
    '#default_value'  => $node->title,
    '#weight'         => -5,
  );
  $form['pid'] = array(
    '#value'          => $node->pid,
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

  $form['#submit'][] = '_openx_node_publisher_submit';
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Publishers
 */
function _openx_node_publisher_insert(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_publisher', $node->openx_node_fields);
}

/**
 * Implementation of hook_load().
 *
 * For Publishers
 */
function _openx_node_publisher_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_publisher} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_publisher_submit(&$form, &$form_state) {
  if ($form_state['values']['op'] != t('Preview')) {
    // Increase the code readability
    $node = &$form_state['values'];

    $data = array(
      'publisherName'   => $node['title'],
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
      $node['pid'] = _openx_manager_api('addPublisher', $data);
    }
    else {
      $data['publisherId'] = (int) $node['pid'];
      _openx_manager_api('modifyPublisher', $data);
    }

    $node['openx_node_fields'] = array(
      'pid'                   => $node['pid'],
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
 * For Publishers
 */
function _openx_node_publisher_update(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_publisher', $node->openx_node_fields, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Publishers
 */
function _openx_node_publisher_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }
}

/**
 * Implementation of hook_load().
 *
 * For Publishers
 */
function _openx_node_publisher_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->content['publisher'] = array(
    '#value'  => theme('publisher', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Publisher fields
 */
function theme_publisher(&$node) {
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
