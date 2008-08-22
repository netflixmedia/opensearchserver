<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Banners.
 */

/**
 * Implementation of hook_access().
 *
 * For Banners
 */
function _openx_node_banner_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create openx banners', $account);
  }

  if ($op == 'update' or $op == 'delete') {
    if (user_access('edit own openx banners', $account) and ($account->uid == $node->uid)) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_delete().
 *
 * For Banners
 */
function _openx_node_banner_delete(&$node) {
  _openx_manager_api('deleteBanner', (int) $node->obid);
  db_query('DELETE FROM {openx_manager_banner} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Banners
 */
function _openx_node_banner_form(&$node, $form_state) {
  $form['#cache'] = FALSE;
  $form['#submit'][] = '_openx_node_banner_submit';
  if (empty($node->nid) and $form_state['post']) {
    $values = &$form_state['post'];
  }
  else {
    $values = (array) $node;
  }

  $form['title'] = array(
    '#default_value'  => $node->title,
    '#required'       => TRUE,
    '#title'          => t('Banner'),
    '#type'           => 'textfield',
    '#weight'         => -5,
  );
  $form['obid'] = array(
    '#value'          => $node->obid,
    '#type'           => 'value',
  );
  if (empty($node->ocid)) {
    $ocid = $_GET['ocid'];
    $campaign = node_load($_GET['nid']);
  }
  else {
    $ocid = $node->ocid;
    $campaign = node_load(db_result(db_query('SELECT nid FROM {openx_manager_campaign}
      WHERE ocid = %d', $ocid)));
  }
  $form['ocid'] = array(
    '#value'          => $ocid,
    '#type'           => 'value',
  );
  $form['campaign'] = array(
    '#value'          => l($campaign->title, 'node/'. $campaign->nid),
    '#title'          => t('Campaign'),
    '#type'           => 'item',
  );
  $form['keywords'] = array(
    '#default_value'  => $node->keywords,
    '#title'          => t('Keywords'),
    '#type'           => 'textfield',
  );
  $form['weight'] = array(
    '#default_value'  => empty($node->weight) ? 1 : $node->weight,
    '#title'          => t('Weight'),
    '#type'           => 'weight',
  );
  $form['destination_url'] = array(
    '#default_value'  => empty($node->destination_url) ? 'http://' : $node->destination_url,
    '#description'    => t(''),
    '#required'       => TRUE,
    '#title'          => t('Destination URL'),
    '#type'           => 'textfield',
  );
  $form['destination_target'] = array(
    '#default_value'  => $node->destination_target,
    '#title'          => t('Destination Target'),
    '#type'           => 'textfield',
  );
  $options = array(
    'sql'   => t('Local banner (SQL)'),
    'web'   => t('Local banner (Webserver)'),
    'url'   => t('External banner'),
    'html'  => t('HTML banner'),
    'txt'   => t('Text ad'),
  );
  if ($node->btype and !empty($node->nid)) {
    $form['btype_text'] = array(
      '#value'          => $options[$node->btype],
      '#title'          => t('Type'),
      '#type'           => 'item',
    );
    $form['btype'] = array(
      '#value'          => $node->btype,
      '#type'           => 'value',
    );
  }
  else {
    $form['btype'] = array(
      '#default_value'  => empty($node->btype) ? 'sql' : $node->btype,
      '#options'        => $options,
      '#required'       => TRUE,
      '#title'          => t('Type'),
      '#type'           => 'select',
    );
  }
  if (empty($node->nid) or $node->btype != 'url') {
    $form['data_url'] = array(
      '#default_value'  => empty($node->data_url) ? 'http://' : $node->data_url,
      '#title'          => t('Image URL'),
      '#type'           => 'textfield',
    );
  }
  $form['data_image'] = array(
    '#value'          => $node->data_image,
    '#type'           => 'value',
  );
  if ($node->btype == 'sql' or $node->btype == 'web') {
    $url = db_result(db_query("SELECT filepath FROM {files} WHERE fid = %d", $node->data_image));
    $form['data_image_preview'] = array(
      '#value'          => theme('image', $url, $node->title),
      '#title'          => t('Current Image'),
      '#type'           => 'item',
    );
  }
  if (empty($node->nid) or $node->btype == 'sql' or $node->btype == 'web') {
    $form['new_data_image'] = array(
      '#title'          => t('Image'),
      '#type'           => 'file',
    );
  }
  if (empty($node->nid) or $node->btype == 'txt' or $node->btype == 'html') {
    $form['data_text'] = array(
      '#default_value'  => $node->data_text,
      '#required'       => ($values['btype'] == 'txt' or $values['btype'] == 'html'),
      '#title'          => t('Text'),
      '#type'           => 'textarea',
    );
  }
  if (empty($node->nid) or $node->btype != 'html') {
    $form['text_status'] = array(
      '#default_value'  => $node->text_status,
      '#title'          => t('Status Text'),
      '#type'           => 'textfield',
    );
  }
  if (empty($node->nid) or $node->btype == 'sql' or $node->btype == 'web' or $node->btype == 'url') {
    $form['text_all'] = array(
      '#default_value'  => $node->text_all,
      '#title'          => t('All Text'),
      '#type'           => 'textfield',
    );
    $form['text_below'] = array(
      '#default_value'  => $node->text_below,
      '#title'          => t('Below Text'),
      '#type'           => 'textfield',
    );
  }
  if (empty($node->nid) or $node->btype == 'html' or $node->btype == 'url') {
    $form['size_height'] = array(
      '#default_value'  => $node->size_height,
      '#description'    => t('Banner height, in pixels.'),
      '#required'       => ($values['btype'] == 'html' or $values['btype'] == 'url'),
      '#size'           => 5,
      '#title'          => t('Height'),
      '#type'           => 'textfield',
    );
    $form['size_width'] = array(
      '#default_value'  => $node->size_width,
      '#description'    => t('Banner width, in pixels.'),
      '#required'       => ($values['btype'] == 'html' or $values['btype'] == 'url'),
      '#size'           => 5,
      '#title'          => t('Width'),
      '#type'           => 'textfield',
    );
  }

  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Banners
 */
function _openx_node_banner_insert(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_banner', $node->openx_node_fields);
}

/**
 * Implementation of hook_load().
 *
 * For Banners
 */
function _openx_node_banner_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_banner} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Unify the hook_insert and hook_update common parts.
 */
function _openx_node_banner_submit(&$form, &$form_state) {
  if ($form_state['values']['op'] != t('Preview')) {
    // Increase the code readability
    $node = &$form_state['values'];

    $data = array(
      'bannerName'      => $node['title'],
      'campaignId'      => (int) $node['ocid'],
      'storageType'     => $node['btype'],
    );
    if ($node['btype'] == 'sql' or $node['btype'] == 'web') {
      $validators = array(
        'file_validate_is_image' => array(),
      );
      if ($file = file_save_upload('new_data_image', $validators, file_directory_path())) {
        // Delete the old image
        if (!empty($node['data_image'])) {
          $file_old = db_fetch_object(db_query('SELECT * FROM {files}
            WHERE fid = %d', $node['data_image']));
          file_delete($file_old->filepath);
          db_query('DELETE FROM {files} WHERE fid = %d', $node['data_image']);
        }
        file_set_status($file, FILE_STATUS_PERMANENT);
        include_once './includes/xmlrpc.inc';
        $node['data_image'] = $file->fid;
        $data['aImage'] = array(
          'filename'  => $file->filename,
          'content'   => xmlrpc_base64(file_get_contents($file->filepath)),
          'editswf'   => FALSE, /// @todo check if it is Flash files
        );
      }
    }
    if ($node['btype'] == 'text') {
      $data['imageURL'] = $node['data_url'];
    }
    if ($node['btype'] == 'html') {
      $data['htmlTemplate'] = $node['data_text'];
    }
    if (!empty($node['size_height'])) {
      $data['height'] = (int) $node['size_height'];
    }
    if (!empty($node['size_width'])) {
      $data['width'] = (int) $node['size_width'];
    }
    if (!empty($node['weight)'])) {
      $data['weight'] = (int) $node['weight'];
    }
    if (!empty($node['destination_url'])) {
      $data['url'] = $node['destination_url'];
    }

    // Update or insert
    if (empty($node['nid'])) {
      $node['obid'] = _openx_manager_api('addBanner', $data);
    }
    else {
      $data['bannerId'] = (int) $node['obid'];
      _openx_manager_api('modifyBanner', $data);
    }

    $node['openx_node_fields'] = array(
      'obid'                 => $node->obid,
      'ocid'                 => $node->ocid,
      'keywords'            => $node->keywords,
      'weight'              => $node->weight,
      'destination_url'     => $node->destination_url,
      'destination_target'  => $node->destination_target,
      'btype'               => $node->btype,
      'text_status'         => $node->text_status,
      'text_all'            => $node->text_all,
      'text_below'          => $node->text_below,
      'data_image'          => $node->data_image,
      'data_text'           => $node->data_text,
      'data_url'            => $node->data_url,
      'size_height'         => $node->size_height,
      'size_width'          => $node->size_width,
    );
  }
}

/**
 * Implementation of hook_update().
 *
 * For Banners
 */
function _openx_node_banner_update(&$node) {
  $node->openx_node_fields['nid'] =  $node->nid;
  drupal_write_record('openx_manager_banner', $node->openx_node_fields, 'nid');
}

/**
 * Implementation of hook_form().
 *
 * For Banners
 */
function _openx_node_banner_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }
}

/**
 * Implementation of hook_load().
 *
 * For Banner
 */
function _openx_node_banner_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->content['openx_banner'] = array(
    '#value'  => theme('openx_banner', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Banner fields
 *
 * @ingroup themable
 */
function theme_openx_banner(&$node) {
  $fields['keywords'] = array(
    '#value'          => check_plain($node->keywords),
    '#title'          => t('Keywords'),
    '#type'           => 'item',
  );
  $fields['weight'] = array(
    '#value'          => check_plain($node->weight),
    '#title'          => t('Weight'),
    '#type'           => 'item',
  );
  $fields['destination_url'] = array(
    '#value'          => check_plain($node->destination_url),
    '#title'          => t('Destination URL'),
    '#type'           => 'item',
  );
  $fields['destination_target'] = array(
    '#value'          => check_plain($node->destination_target),
    '#title'          => t('Destination Target'),
    '#type'           => 'item',
  );
  $options = array(
    'sql'   => t('Local banner (SQL)'),
    'web'   => t('Local banner (Webserver)'),
    'url'   => t('External banner'),
    'html'  => t('HTML banner'),
    'txt'   => t('Text ad'),
  );
  $fields['btype'] = array(
    '#value'          => $options[$node->btype],
    '#title'          => t('Type'),
    '#type'           => 'item',
  );
  if ($node->btype != 'url') {
    $form['data_url'] = array(
      '#value'          => check_plain($node->data_url),
      '#title'          => t('Image URL'),
      '#type'           => 'item',
    );
  }
  if ($node->btype == 'sql' or $node->btype == 'web') {
    $url = db_result(db_query("SELECT filepath FROM {files}
      WHERE fid = %d", $node->data_image));
    $fields['data_image_preview'] = array(
      '#value'          => theme('image', $url, $node->title),
      '#title'          => t('Image'),
      '#type'           => 'item',
    );
  }
  if ($node->btype == 'txt' or $node->btype == 'html') {
    $fields['data_text'] = array(
      '#value'          => check_plain($node->data_text),
      '#title'          => t('Text'),
      '#type'           => 'item',
    );
  }
  if ($node->btype != 'html') {
    $fields['text_status'] = array(
      '#value'          => check_plain($node->text_status),
      '#title'          => t('Status Text'),
      '#type'           => 'item',
    );
  }
  if ($node->btype == 'sql' or $node->btype == 'web' or $node->btype == 'url') {
    $fields['text_all'] = array(
      '#value'          => check_plain($node->text_all),
      '#title'          => t('All Text'),
      '#type'           => 'item',
    );
    $fields['text_below'] = array(
      '#value'          => check_plain($node->text_below),
      '#title'          => t('Below Text'),
      '#type'           => 'item',
    );
  }
  if ($node->btype == 'html' or $node->btype == 'url') {
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
  }

  // Print all field in the right order
  $output = '';
  foreach ($fields as $field) {
    $output .= drupal_render($field);
  }
  return $output;
}

/**
 * Display the Banner fields
 *
 * @ingroup themable
 */
function theme_openx_banner_form(&$form) {
  // Add the Javascript if its a new banner
  if (empty($form['#node']->nid)) {
    drupal_add_js(drupal_get_path('module', 'openx_manager') .'/openx_manager.banner.js');
  }

  return drupal_render($form);
}
