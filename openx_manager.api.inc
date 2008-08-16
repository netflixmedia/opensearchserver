<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * All direct API communications.
 */

/**
 * Ask, if not already, a new session ID from OpenX
 *
 * @param $check_only
 *   Boolean. TRUE if its only to check if the session was already opened.
 * @return
 *   String. The full session ID, used on every further communication.
 */
function _openx_manager_session($check_only = FALSE) {
  static $session_id;
  if (!$check_only) {
    $session_id = xmlrpc(
      'http://'. variable_get('openx_manager_server', '') .'/www/api/v1/xmlrpc/LogonXmlRpcService.php',
      'logon',
      variable_get('openx_manager_username', ''),
      variable_get('openx_manager_password', '')
    );
  }
  return $session_id;
}
