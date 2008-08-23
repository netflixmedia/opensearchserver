<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Module settings.
 */

/**
 * Module settings.
 *
 * @ingroup form
 */
function _openx_manager_settings() {
  $form['openx_manager_server'] = array(
    '#default_value'  => variable_get('openx_manager_server', ''),
    '#title'          => t('Server'),
    '#type'           => 'textfield',
  );
  $form['openx_manager_username'] = array(
    '#default_value'  => variable_get('openx_manager_username', ''),
    '#title'          => t('Username'),
    '#type'           => 'textfield',
  );
  $form['openx_manager_password'] = array(
    '#default_value'  => variable_get('openx_manager_password', ''),
    '#title'          => t('Password'),
    '#type'           => 'password',
  );
  $form['#validate'][] = '_openx_manager_settings_validate';
  return system_settings_form($form);
}

/**
 * Validate the settings data.
 *
 * @ingroup form
 */
function _openx_manager_settings_validate(&$form, &$form_state) {
  // Filter the server URL
  $url = trim($form_state['values']['openx_manager_server']);
  if (substr($url, 0, 7) == 'http://') {
    $url = substr($url, 7);
  }
  elseif (substr($url, 0, 8) == 'http://') {
    $url = substr($url, 8);
  }
  if (substr($url, -1, 1) == '/') {
    $url = substr($url, 0, -1);
  }
  form_set_value($form['openx_manager_server'], $url, $form_state);
}

/**
 * Import data from the OpenX server
 */
function _openx_manager_import() {
  $form['ntype'] = array(
    '#options'  => array(
      'advertiser'  => t('Advertiser'),
      'banner'      => t('Banner'),
      'campaign'    => t('Campaign'),
      'manager'     => t('Manager'),
      'publisher'   => t('Publisher'),
      'user'        => t('User'),
      'zone'        => t('Zone'),
    ),
    '#required' => TRUE,
    '#title'    => t('Import'),
    '#type'     => 'radios',
  );
  $form['submit'] = array(
    '#value'    => t('Import'),
    '#type'     => 'submit',
  );

  return $form;
}

/**
 * Import data from the OpenX server
 */
function _openx_manager_import_submit(&$form, &$form_state) {
  $ntype = $ntype2 = $form_state['values']['ntype'];
  $sql_id     = 'o'. substr($ntype, 0, 1) .'id';
  $sql_table  = 'openx_manager_'. $ntype;
  switch ($ntype) {
    case 'advertiser':
      $api_op     = 'getAdvertiserListByAgencyId';
      $sql_id2    = 'omid';
      $sql_table2 = 'openx_manager_manager';
      $fields     = array(
        'title'                 => 'advertiserName',
        'info_contact'          => 'contactName',
        'info_email'            => 'emailAddress',
        'oaid'                  => 'advertiserId',
      );
      break;
    case 'banner':
      $api_op     = 'getBannerListByCampaignId';
      $sql_id2    = 'ocid';
      $sql_table2 = 'openx_manager_campaign';
      $fields     = array(
        'title'                 => 'bannerName',
        'obid'                  => 'bannerId',
        'ocid'                  => 'campaignId',
        'btype'                 => 'storageType',
        'data_url'              => 'imageURL',
        'data_text'             => 'htmlTemplate',
        'size_height'           => 'height',
        'size_width'            => 'width',
      );
      break;
    case 'campaign':
      $api_op     = 'getCampaignListByAdvertiserId';
      $sql_id2    = 'oaid';
      $sql_table2 = 'openx_manager_advertiser';
      $fields     = array(
        'title'                 => 'campaignName',
        'ocid'                  => 'campaignId',
        'contract_activation'   => 'startDate',
        'contract_expiration'   => 'endDate',
        'inventory_impressions' => 'impressions',
        'inventory_clicks'      => 'clicks',
        'contract_expiration'   => 'priority',
        'distribution_weight'   => 'weight',
      );
      break;
    case 'manager':
      $api_op     = 'getAgencyList';
      $ntype2     = 'agency';
      $fields     = array(
        'title'                 => 'agencyName',
        'omid'                  => 'agencyId',
        'accountId'             => 'accountId',
        'contact'               => 'contactName',
        'email'                 => 'emailAddress',
      );
      break;
    case 'publisher':
      $api_op     = 'getPublisherListByAgencyId';
      $sql_id2    = 'omid';
      $sql_table2 = 'openx_manager_manager';
      $fields     = array(
        'title'                 => 'publisherName',
        'contact'               => 'contactName',
        'email'                 => 'emailAddress',
        'opid'                  => 'publisherId',
      );
      break;
    case 'user':
      $api_op     = 'getUserListByAccountId';
      $sql_id2    = 'oacid';
      $sql_table2 = 'openx_manager_manager';
      $fields     = array(
        'title'                 => 'userName',
        'contact'               => 'contactName',
        'email'                 => 'emailAddress',
        'ouid'                  => 'userId',
      );
      break;
    case 'zone':
      $api_op     = 'getZoneListByPublisherId';
      $sql_id2    = 'opid';
      $sql_table2 = 'openx_manager_publisher';
      $fields     = array(
        'title'                 => 'zoneName',
        'ouid'                  => 'zoneId',
        'opid'                  => 'publisherId',
        'size_height'           => 'height',
        'size_width'            => 'width',
        'zone_type'             => 'type',
      );
      break;
  }

  // Get the existing nodes
  $existing_oids = array();
  if ($sql_existing_oids = db_query("SELECT %s FROM {%s}", $sql_id, $sql_table)) {
    while ($sql_existing_oid = db_fetch_array($sql_existing_oids)) {
      $existing_oids[$sql_existing_oid[$sql_id]] = $sql_existing_oid[$sql_id];
    }
  }
  unset($sql_existing_oids);

  global $user;

  // Check if it requires a upper ID
  if ($sql_id2) {
    $oids = db_query("SELECT %s FROM {%s}", $sql_id2, $sql_table2);
    while ($oid = db_result($oids)) {
      if ($nodes = _openx_manager_api($api_op, (int) $oid)) {
        foreach ($nodes as $node) {
          if (empty($existing_oids[$node[$ntype2 .'Id']])) {
            $new_node = array(
              'type'    => 'openx_'. $ntype,
              'uid'     => $user->uid,
              'name'    => $user->name,
            );
            foreach ($fields as $index => $value) {
              $new_node[$index] = $node[$value];
            }
            $new_node = node_submit($new_node);
            node_save($new_node);
            drupal_set_message(t('Created a new %type: %title', array(
              '%title'  => $new_node->title,
              '%type'   => $new_node->type,
            )));
          }
        }
      }
    }
  }
  else {
    if ($nodes = _openx_manager_api($api_op)) {
      foreach ($nodes as $node) {
        if (empty($existing_oids[$node[$ntype2 .'Id']])) {
          $new_node = array(
            'type'    => 'openx_'. $ntype,
            'uid'     => $user->uid,
            'name'    => $user->name,
          );
          foreach ($fields as $index => $value) {
            $new_node[$index] = $node[$value];
          }
          $new_node = node_submit($new_node);
          node_save($new_node);
          drupal_set_message(t('Created a new %type: %title', array(
            '%title'  => $new_node->title,
            '%type'   => $new_node->type,
          )));
        }
      }
    }
  }
}
