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
  return system_settings_form($form);
}
