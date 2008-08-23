<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Zones.
 */

define('OX_ZONE_BANNER',        0);
define('OX_ZONE_INTERSTITIAL',  1);
define('OX_ZONE_POPUP',         2);
define('OX_ZONE_TEXT',          3);
define('OX_ZONE_EMAIL',         4);

/**
 * Implementation of hook_access().
 *
 * For Zones
 */
function _openx_node_zone_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create openx zones', $account);
  }

  if ($op == 'update' or $op == 'delete') {
    if (user_access('edit own openx zones', $account) and ($account->uid == $node->uid)) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_delete().
 *
 * For Zones
 */
function _openx_node_zone_delete(&$node) {
  _openx_manager_api('deleteZone', (int) $node->ozid);
  db_query('DELETE FROM {openx_manager_zone} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Zones
 */
function _openx_node_zone_form(&$node, $form_state) {
  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Zone'),
    '#required'       => TRUE,
    '#default_value'  => $node->title,
    '#weight'         => -5,
  );
  $form['ozid'] = array(
    '#value'          => $node->ozid,
    '#type'           => 'value',
  );
  if (empty($node->opid)) {
    $opid = $_GET['opid'];
    $publisher = node_load($_GET['nid']);
  }
  else {
    $opid = $node->opid;
    $publisher = node_load(db_result(db_query('SELECT nid FROM {openx_manager_publisher}
      WHERE opid = %d', $opid)));
  }
  $form['opid'] = array(
    '#value'          => $opid,
    '#type'           => 'value',
  );
  $form['publisher'] = array(
    '#value'          => l($publisher->title, 'node/'. $publisher->nid),
    '#title'          => t('Publisher'),
    '#type'           => 'item',
  );
  $form['zone_type'] = array(
    '#default_value'  => empty($node->zone_type) ? OX_ZONE_BANNER : $node->zone_type,
    '#options'        => array(
      OX_ZONE_BANNER        => t('Banner, Button or Rectangle'),
      OX_ZONE_INTERSTITIAL  => t('Interstitial or Floating DHTML'),
      OX_ZONE_POPUP         => t('Popup'),
      OX_ZONE_TEXT          => t('Text'),
      OX_ZONE_EMAIL         => t('Email or Newsletter'),
    ),
    '#type'           => 'select',
  );
  $form['size_height'] = array(
    '#default_value'  => $node->size_height,
    '#description'    => t('Banner height, in pixels.'),
    '#size'           => 5,
    '#title'          => t('Height'),
    '#type'           => 'textfield',
  );
  $form['size_width'] = array(
    '#default_value'  => $node->size_width,
    '#description'    => t('Banner width, in pixels.'),
    '#size'           => 5,
    '#title'          => t('Width'),
    '#type'           => 'textfield',
  );

  $form['#submit'][] = '_openx_node_zone_submit';
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Zones
 */
function _openx_node_zone_insert(&$node) {
  drupal_write_record('openx_manager_zone', $node);
}

/**
 * Implementation of hook_load().
 *
 * For Zones
 */
function _openx_node_zone_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_zone} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_zone_submit(&$form, &$form_state) {
  if ($form_state['values']['op'] != t('Preview')) {
    // Increase the code readability
    $node = &$form_state['values'];

    $data = array(
      'zoneName'    => $node['title'],
      'publisherId' => (int) $node['opid'],
      'type'        => (int) $node['zone_type'],
    );
    if (!empty($node['size_width']) and !empty($node['size_height'])) {
      $data['width']    = (int) $node['size_width'];
      $data['height']   = (int) $node['size_height'];
    }

    // Update or insert
    if (empty($node['nid'])) {
      $node['ozid'] = _openx_manager_api('addZone', $data);
    }
    else {
      $data['zoneId'] = (int) $node['ozid'];
      _openx_manager_api('modifyZone', $data);
    }
  }
}

/**
 * Implementation of hook_update().
 *
 * For Zones
 */
function _openx_node_zone_update(&$node) {
  drupal_write_record('openx_manager_zone', $node, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Zones
 */
function _openx_node_zone_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }

  // Check if the integer values are really integers
  $integers_values = array(
    'size_width',
    'size_height',
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
 * For Zones
 */
function _openx_node_zone_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->content['openx_zone'] = array(
    '#value'  => theme('openx_zone', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Zone fields
 */
function theme_openx_zone(&$node) {
  $publisher = node_load(db_result(db_query('SELECT nid FROM {openx_manager_publisher}
    WHERE opid = %d', $node->opid)));
  $fields['publisher'] = array(
    '#value'          => l($publisher->title, 'node/'. $publisher->nid),
    '#title'          => t('Publisher'),
    '#type'           => 'item',
  );
  $options = array(
    OX_ZONE_BANNER        => t('Banner, Button or Rectangle'),
    OX_ZONE_INTERSTITIAL  => t('Interstitial or Floating DHTML'),
    OX_ZONE_POPUP         => t('Popup'),
    OX_ZONE_TEXT          => t('Text'),
    OX_ZONE_EMAIL         => t('Email or Newsletter'),
  );
  $fields['zone_type'] = array(
    '#value'          => $options[$node->zone_type],
    '#type'           => 'item',
  );
  $fields['size_height'] = array(
    '#value'          => t('!size pixels', array('!size' => $node->size_height)),
    '#title'          => t('Height'),
    '#type'           => 'item',
  );
  $fields['size_width'] = array(
    '#value'          => t('!size pixels', array('!size' => $node->size_width)),
    '#title'          => t('Width'),
    '#type'           => 'item',
  );

  // Print all field in the right order
  $output = '';
  foreach ($fields as $field) {
    $output .= drupal_render($field);
  }
  return $output;
}
