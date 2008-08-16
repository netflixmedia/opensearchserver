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
 * @param $file
 *   String. The XML-RPC file that should be called on OpenX server
 * @param $op
 *   String. The XML-RPC operation that should be called.
 * @param ...
 *   Any. All arguments that will be passed to the XML-RPC call.
 * @return
 *   String. The full session ID, used on every further communication.
 */
function _openx_manager_call($file, $op) {
  // Get all function arguments
  $args = func_get_args();

  // Replace the first two by the URL and the XML-RPC operation and session ID
  array_shift($args);
  array_shift($args);
  array_unshift($args, _openx_manager_session());
  array_unshift($args, $op);
  array_unshift($args, 'http://'. variable_get('openx_manager_server', '') ."/www/api/v1/xmlrpc/{$file}XmlRpcService.php");

  // Call the XML-RPC
  return call_user_func_array('xmlrpc', $args);
}
