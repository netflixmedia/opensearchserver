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
