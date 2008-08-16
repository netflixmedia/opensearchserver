<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Module settings.
 */

/**
 * Module settings.
 */
function _openx_manager_settings() {
  $form['openx_manager_server'] = array(
    '#default_value'  => variable_get('openx_manager_server', ''),
    '#title'          => t('Server'),
    '#type'           => 'textfield',
  );
  return system_settings_form($form);
}
