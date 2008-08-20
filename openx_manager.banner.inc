<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Banners.
 */

/**
 * Implementation of hook_delete().
 *
 * For Banners
 */
function _openx_node_banner_delete(&$node) {
  _openx_manager_api('deleteBanner', (int) $node->bid);
  db_query('DELETE FROM {openx_manager_banner} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Banners
 */
function _openx_node_banner_form(&$node, $form_state) {
  // Add the Javascript if its a new banner
  if (empty($node->bid)) {
    drupal_add_js(drupal_get_path('module', 'openx_manager') .'/openx_manager.banner.js');
  }

  $form['title'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Banner'),
    '#required'       => TRUE,
    '#default_value'  => $node->title,
    '#weight'         => -5,
  );
  $form['bid'] = array(
    '#value'          => $node->bid,
    '#type'           => 'value',
  );
  if (empty($node->cid)) {
    $cid = $_GET['cid'];
    $campaign = node_load($_GET['nid']);
  }
  else {
    $cid = $node->cid;
    $campaign = node_load(db_result(db_query('SELECT nid FROM {openx_manager_campaign}
      WHERE cid = %d', $cid)));
  }
  $form['cid'] = array(
    '#value'          => $cid,
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
  if ($node->btype) {
    $form['text_status'] = array(
      '#value'          => $options[$node->btype],
      '#title'          => t('Type'),
      '#type'           => 'item',
    );
  }
  else {
    $form['btype'] = array(
      '#default_value'  => 'sql',
      '#options'        => $options,
      '#required'       => TRUE,
      '#title'          => t('Type'),
      '#type'           => 'select',
    );
  }
  if (empty($node->btype) or $node->btype == 'sql' or $node->btype == 'web') {
    $form['data_image'] = array(
      '#default_value'  => $node->data_image,
      '#required'       => ($node->btype == 'sql' or $node->btype == 'web'),
      '#title'          => t('Image'),
      '#type'           => 'file',
    );
  }
  if (empty($node->btype) or $node->btype == 'txt' or $node->btype == 'html') {
    $form['data_text'] = array(
      '#default_value'  => $node->data_text,
      '#required'       => ($node->btype == 'txt' or $node->btype == 'html'),
      '#title'          => t('Text'),
      '#type'           => 'textarea',
    );
  }
  if (empty($node->btype) or $node->btype != 'html') {
    $form['text_status'] = array(
      '#default_value'  => $node->text_status,
      '#title'          => t('Status Text'),
      '#type'           => 'textfield',
    );
  }
  if (empty($node->btype) or $node->btype == 'sql' or $node->btype == 'web' or $node->btype == 'url') {
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
  if (empty($node->btype) or $node->btype == 'html' or $node->btype == 'url') {
    $form['size_height'] = array(
      '#default_value'  => $node->size_height,
      '#description'    => t('Banner height, in pixels.'),
      '#required'       => ($node->btype == 'html' or $node->btype == 'url'),
      '#size'           => 5,
      '#title'          => t('Height'),
      '#type'           => 'textfield',
    );
    $form['size_width'] = array(
      '#default_value'  => $node->size_width,
      '#description'    => t('Banner width, in pixels.'),
      '#required'       => ($node->btype == 'html' or $node->btype == 'url'),
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
  $data = array(
    'bannerName'      => $node->title,
    'campaignId'      => (int) $node->cid,
    'storageType'     => $node->btype,
  );
  if ($node->btype == 'sql' or $node->btype == 'web') {
    $data['fileName'] = $node->data_image;
  }
  if ($node->btype == 'text') {
    $data['imageURL'] = $node->data_url;
  }
  if ($node->btype == 'html') {
    $data['htmlTemplate'] = $node->data_text;
  }
  if (!empty($node->size_height)) {
    $data['height'] = (int) $node->size_height;
  }
  if (!empty($node->size_width)) {
    $data['width'] = (int) $node->size_width;
  }
  if (!empty($node->weight)) {
    $data['weight'] = (int) $node->weight;
  }
  if (!empty($node->destination_url)) {
    $data['url'] = $node->destination_url;
  }
  $node->bid = _openx_manager_api('addBanner', $data);

  db_query("INSERT INTO {openx_manager_banner}
    (nid, bid, cid,
    keywords, weight, destination_url, destination_target,
    text_status, btype, upload_image, text_text,
    text_all, text_below, size_height, size_width)
    VALUES (%d, %d, %d,
    '%s', %d, '%s', '%s',
    '%s', '%s', '%s', '%s',
    '%s', '%s', %d, %d)",
    $node->nid, $node->bid, $node->cid,
    $node->keywords, $node->weight, $node->destination_url, $node->destination_target,
    $node->text_status, $node->btype, $node->upload_image, $node->text_text,
    $node->text_all, $node->text_below, $node->size_height, $node->size_width
  );
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
 * Implementation of hook_update().
 *
 * For Banners
 */
function _openx_node_banner_update(&$node) {
  $data = array(
    'bannerId'        => (int) $node->bid,
    'bannerName'      => $node->title,
    'campaignId'      => (int) $node->cid,
    'storageType'     => $node->btype,
  );
  if ($node->btype == 'sql' or $node->btype == 'web') {
    $data['fileName'] = $node->data_image;
  }
  if ($node->btype == 'text') {
    $data['imageURL'] = $node->data_url;
  }
  if ($node->btype == 'html') {
    $data['htmlTemplate'] = $node->data_text;
  }
  if (!empty($node->size_height)) {
    $data['height'] = (int) $node->size_height;
  }
  if (!empty($node->size_width)) {
    $data['width'] = (int) $node->size_width;
  }
  if (!empty($node->weight)) {
    $data['weight'] = (int) $node->weight;
  }
  if (!empty($node->destination_url)) {
    $data['url'] = $node->destination_url;
  }
  _openx_manager_api('modifyBanner', $data);

  db_query("UPDATE {openx_manager_banner} SET
    bid = %d, cid = %d,
    keywords = '%s', weight = %d, destination_url = '%s', destination_target = '%s',
    text_status = '%s', btype = '%s', upload_image = '%s', text_text = '%s',
    text_all = '%s', text_below = '%s', size_height = %d, size_width = %d
    WHERE nid = %d",
    $node->nid, $node->bid, $node->cid,
    $node->keywords, $node->weight, $node->destination_url, $node->destination_target,
    $node->text_status, $node->btype, $node->upload_image, $node->text_text,
    $node->text_all, $node->text_below, $node->size_height, $node->size_width
  );
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
  if (!empty($node->report_days) and !is_numeric($node->report_days)) {
    form_set_error('report_days', t('It must be numeric'));
  }
}

/**
 * Implementation of hook_load().
 *
 * For Banner
 */
function _openx_node_banner_view(&$node, $teaser = FALSE, $page = FALSE) {
  $node->content['banner'] = array(
    '#value'  => theme('banner', $node),
    '#weight' => 1,
  );

  return $node;
}

/**
 * Display the Banner fields
 */
function theme_banner(&$node) {
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
  if ($node->btype == 'sql' or $node->btype == 'web') {
    $fields['upload_image'] = array(
      '#value'          => check_plain($node->upload_image),
      '#title'          => t('Image'),
      '#type'           => 'item',
    );
  }
  if ($node->btype == 'txt' or $node->btype == 'html') {
    $fields['text_text'] = array(
      '#value'          => check_plain($node->text_text),
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
      '#value'          => check_plain($node->size_height),
      '#title'          => t('Height'),
      '#type'           => 'item',
    );
    $fields['size_width'] = array(
      '#value'          => check_plain($node->size_width),
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
