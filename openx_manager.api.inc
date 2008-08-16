<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * All direct API communications.
 */

/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * All direct API communications.
 */
function openx_manager_session() {
  static $session_id = xmlrpc(
    'http://'. variable_get('openx_manager_server', '') .'/www/api/v1/xmlrpc/LogonXmlRpcService.php',
    'logon',
    variable_get('openx_manager_username', ''),
    variable_get('openx_manager_password', ''));
  return $session_id;
}
