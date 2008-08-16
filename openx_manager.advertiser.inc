<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Manage all OpenX Advertisers.
 */

function _openx_manager_advertiver_nodeapi(&$node, $op, $teaser, $page) {
  switch ($op) {
    case 'form':
      $form['info'] = array(
        '#title'          => t('Contact'),
        '#type'           => 'fieldset',
      );
      $form['info']['info_contact'] = array(
        '#default_value'  => $node->info_contact,
        '#title'          => t('Contact name'),
        '#type'           => 'textfield',
      );
      $form['info']['info_email'] = array(
        '#default_value'  => $node->info_email,
        '#title'          => t('Contact email'),
        '#type'           => 'textfield',
      );
      $form['report'] = array(
        '#title'          => t('Advertiser report'),
        '#type'           => 'fieldset',
      );
      $form['report']['report_auto'] = array(
        '#default_value'  => $node->report_auto,
        '#title'          => t('Email when a campaign is automatically activated/deactivated'),
        '#type'           => 'checkbox',
      );
      $form['report']['report_email'] = array(
        '#default_value'  => $node->report_email,
        '#title'          => t('Email campaign delivery reports'),
        '#type'           => 'checkbox',
      );
      $form['report']['report_days'] = array(
        '#description'    => t('Number of days between campaign delivery reports'),
        '#default_value'  => $node->report_days,
        '#title'          => t('Number of days'),
        '#type'           => 'textfield',
      );
      $form['misc'] = array(
        '#title'          => t('Miscellaneous'),
        '#type'           => 'fieldset',
      );
      $form['misc']['misc_only_one_banner'] = array(
        '#default_value'  => $node->misc_only_one_banner,
        '#title'          => t('Display only one banner from this advertiser on a web page'),
        '#type'           => 'checkbox',
      );
      return $form;
    case 'load':
      $sql = db_query('SELECT * FROM {openx_advertiser} WHERE nid = %d', $node->nid);
      while ($field = db_fetch_array($sql)) {
        foreach ($field as $key => $value) {
          $node->$key = $value;
        }
      }
      return;
    case 'validate':
    case 'submit':
  }
}
