<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Advertisers.
 */

/**
 * Implementation of hook_delete().
 *
 * For Advertisers
 */
function _openx_node_advertiser_delete(&$node) {
  _openx_manager_api('deleteAdvertiser', (int) $node->aid);
  db_query('DELETE FROM {openx_manager_advertiser} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Advertisers
 */
function _openx_node_advertiser_form(&$node, $form_state) {
  $form['aid'] = array(
    '#value'          => $node->aid,
    '#type'           => 'value',
  );
  $form['info'] = array(
    '#title'          => t('Contact'),
    '#type'           => 'fieldset',
  );
  $form['info']['info_contact'] = array(
    '#default_value'  => $node->info_contact,
    '#required'       => TRUE,
    '#title'          => t('Contact name'),
    '#type'           => 'textfield',
  );
  $form['info']['info_email'] = array(
    '#default_value'  => $node->info_email,
    '#title'          => t('Contact email'),
    '#type'           => 'textfield',
  );
//   $form['report'] = array(
//     '#title'          => t('Advertiser report'),
//     '#type'           => 'fieldset',
//   );
  $form['report']['report_auto'] = array(
    '#default_value'  => $node->report_auto,
    '#title'          => t('Email when a campaign is automatically activated/deactivated'),
//     '#type'           => 'checkbox',
    '#type'           => 'value',
  );
  $form['report']['report_email'] = array(
    '#default_value'  => $node->report_email,
    '#title'          => t('Email campaign delivery reports'),
//     '#type'           => 'checkbox',
    '#type'           => 'value',
  );
  $form['report']['report_days'] = array(
    '#description'    => t('Number of days between campaign delivery reports'),
    '#default_value'  => $node->report_days,
    '#title'          => t('Number of days'),
//     '#type'           => 'textfield',
    '#type'           => 'value',
  );
//   $form['misc'] = array(
//     '#title'          => t('Miscellaneous'),
//     '#type'           => 'fieldset',
//   );
  $form['misc']['misc_only_one_banner'] = array(
    '#default_value'  => $node->misc_only_one_banner,
    '#title'          => t('Display only one banner from this advertiser on a web page'),
//     '#type'           => 'checkbox',
    '#type'           => 'value',
  );
  return $form;
}

/**
 * Implementation of hook_insert().
 *
 * For Advertisers
 */
function _openx_node_advertiser_insert(&$node) {
  $data = array(
    'advertiserName'  => ($node->title) ? $node->title : "test". rand(),
    'contactName'     => $node->info_contact,
  );
  if (!empty($node->info_email)) {
    $data['emailAddress'] = $node->info_email;
  }
  $node->aid = _openx_manager_api('addAdvertiser', $data);
dpm($node->aid);
  $node->aid = (int) $node->aid;
_openx_manager_call_error();
  db_query("INSERT INTO {openx_manager_advertiser}
    (nid, aid, info_contact, info_email, report_auto, report_email, report_days, misc_only_one_banner)
    VALUES (%d, %d, '%s', '%s', %d, %d, %d, %d)",
    $node->nid, $node->aid,
    $node->info_contact, $node->info_email, $node->report_auto, $node->report_email,
    $node->report_days, $node->misc_only_one_banner);
}

/**
 * Implementation of hook_load().
 *
 * For Advertisers
 */
function _openx_node_advertiser_load(&$node) {
  $sql = db_query('SELECT * FROM {openx_manager_advertiser} WHERE nid = %d', $node->nid);
  while ($field = db_fetch_array($sql)) {
    foreach ($field as $key => $value) {
      $node->$key = $value;
    }
  }
}

/**
 * Implementation of hook_update().
 *
 * For Advertisers
 */
function _openx_node_advertiser_update(&$node) {
  $data = array(
    'advertiserId'    => $node->aid,
    'advertiserName'  => ($node->title) ? $node->title : "test". rand(),
    'contactName'     => $node->info_contact,
  );
  if (!empty($node->info_email)) {
    $data['emailAddress'] = $node->info_email;
  }
  _openx_manager_api('modifyAdvertiser', $data);
dpm($data);
_openx_manager_call_error();
  db_query("UPDATE {openx_manager_advertiser} SET
    aid = %d, info_contact = '%s', info_email = '%s', report_auto = '%s',
    report_email = %d, report_days = %d, misc_only_one_banner = %d
    WHERE nid = %d",
    $node->aid,
    $node->info_contact, $node->info_email, $node->report_auto, $node->report_email,
    $node->report_days, $node->misc_only_one_banner, $node->nid);
}

/**
 * Implementation of hook_form().
 *
 * For Advertisers
 */
function _openx_node_advertiser_validate(&$node) {
  if (!variable_get('openx_manager_server', '')) {
    form_set_error('title', t('OpenX integration not configured'));
  }
  if (!empty($node->report_days) and !is_numeric($node->report_days)) {
    form_set_error('report_days', t('It must be numeric'));
  }
}
