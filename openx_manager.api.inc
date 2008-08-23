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
 * @param $op
 *   String. The XML-RPC operation that should be called.
 * @param ...
 *   Any. All arguments that will be passed to the XML-RPC call.
 * @return
 *   String. The full session ID, used on every further communication.
 */
function _openx_manager_call($op) {
  // Get all function arguments
  $args = func_get_args();

  // Replace the first two by the URL and the XML-RPC operation and session ID
  array_shift($args);
  array_unshift($args, _openx_manager_session());
  array_unshift($args, $op);
  array_unshift($args, 'http://'. variable_get('openx_manager_server', '') . _openx_manager_call_file($op));

  // Call the XML-RPC
  $result = call_user_func_array('xmlrpc', $args);

  // Show errors, if any
  if ($error_number = xmlrpc_errno()) {
    if (function_exists('dpm')) {
      $func = 'dpm';
    }
    else {
      $func = 'drupal_set_message';
    }
    $func($error_number, 'error');
    $func(xmlrpc_error_msg(), 'error');
  }

  return $result;
}

/**
 * Find out what file should be called.
 *
 * @param $op
 *   String. The XML-RPC operation that should be called.
 * @return
 *   String. The full session ID, used on every further communication.
 */
function _openx_manager_call_file($op) {
  switch ($op) {
    case 'addAdvertiser':
    case 'deleteAdvertiser':
    case 'getAdvertiser':
    case 'getAdvertiserBannerStatistics':
    case 'getAdvertiserCampaignStatistics':
    case 'getAdvertiserDailyStatistics':
    case 'getAdvertiserListByAgencyId':
    case 'getAdvertiserPublisherStatistics':
    case 'getAdvertiserZoneStatistics':
    case 'modifyAdvertiser':
      return '/www/api/v1/xmlrpc/AdvertiserXmlRpcService.php';
    case 'addCampaign':
    case 'deleteCampaign':
    case 'getCampaign':
    case 'getCampaignBannerStatistics':
    case 'getCampaignDailyStatistics':
    case 'getCampaignListByAdvertiserId':
    case 'getCampaignPublisherStatistics':
    case 'getCampaignZoneStatistics':
    case 'modifyCampaign':
      return '/www/api/v1/xmlrpc/CampaignXmlRpcService.php';
    case 'addBanner':
    case 'deleteBanner':
    case 'getBanner':
    case 'getBannerDailyStatistics':
    case 'getBannerListByCampaignId':
    case 'getBannerPublisherStatistics':
    case 'getBannerZoneStatistics':
    case 'modifyBanner':
      return '/www/api/v1/xmlrpc/BannerXmlRpcService.php';
    case 'addAgency':
    case 'deleteAgency':
    case 'modifyAgency':
    case 'getAgency':
      return '/www/api/v1/xmlrpc/AgencyXmlRpcService.php';
    case 'addPublisher':
    case 'deletePublisher':
    case 'modifyPublisher':
    case 'getPublisher':
      return '/www/api/v1/xmlrpc/PublisherXmlRpcService.php';
    case 'addUser':
    case 'deleteUser':
    case 'modifyUser':
    case 'getUser':
      return '/www/api/v1/xmlrpc/UserXmlRpcService.php';
    case 'addZone':
    case 'deleteZone':
    case 'generateTags':
    case 'getZone':
    case 'getZoneAdvertiserStatistics':
    case 'getZoneBannerStatistics':
    case 'getZoneCampaignStatistics':
    case 'getZoneDailyStatistics':
    case 'getZoneListByPublisherId':
    case 'linkBanner':
    case 'linkCampaign':
    case 'modifyZone':
    case 'unlinkBanner':
    case 'unlinkCampaign':
      return '/www/api/v1/xmlrpc/ZoneXmlRpcService.php';
  }
}
