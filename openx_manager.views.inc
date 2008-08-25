<?
// $Id$
/**
 * @author Bruno Massa http://drupal.org/user/67164
 * @file
 * Views integration.
 */

/**
 * Implementation of hook_views_data().
 */
function openx_manager_views_data() {
  // Get the standart
  $data = _openx_manager_views_data();

  $data['']['']['field'] = '';

  return $data;
}

/**
 * Helper function for openx_manager_views_data(). It scans all fields
 * from the module DB tables and export a Views field list.
 *
 * @return
 *   Array. Each table and eachfield from hook_schema, in the way Views
 *   module use.
 */
function _openx_manager_views_data() {
  // Get all fields from hook_schema
  module_load_include('install', 'openx_manager');
  $sql_tables = openx_manager_schema();

  foreach ($sql_tables as $table => $sql_fields) {
    // Basic table definitions
    $data[$table]['table']['group']  = t('OpenX !type', array('!type' => $sql_fields['description']));
    $data[$table]['table']['join'] = array(
      'node' => array(
        'left_field'  => 'nid',
        'field'       => 'nid',
      ),
    );

    // Scan all fields
    foreach ($sql_fields['fields'] as $field_name => $field_data) {
      if ($field_name != 'nid') {
        if ($field_name == 'contract_expiration' or $field_name == 'contract_activation') {
          $handler_field  = 'views_handler_field_date';
          $handler_sort   = 'views_handler_sort_date';
          $handler_filter = 'views_handler_filter_date';
        }
        elseif ($field_data['type'] == 'int' and $field_data['size'] == 'tiny') {
          $handler_field  = 'views_handler_field_boolean';
          $handler_sort   = 'views_handler_sort';
          $handler_filter = 'views_handler_filter_boolean_operator';
        }
        elseif ($field_data['type'] == 'int') {
          $handler_field  = 'views_handler_field_numeric';
          $handler_sort   = 'views_handler_sort';
          $handler_filter = 'views_handler_filter_numeric';
        }
        else {
          $handler_field  = 'views_handler_field';
          $handler_sort   = 'views_handler_sort';
          $handler_filter = 'views_handler_filter';
        }

        // Use the default
        $data[$table][$field_name] = array(
          'title'     => (empty($field_data['title']) ? $field_name : $field_data['title']),
          'help'      => (empty($field_data['description']) ? ' ' : $field_data['description']),
          'field'     => array(
            'handler'         => $handler_field,
            'click sortable'  => TRUE,
          ),
          'sort'      => array(
            'handler'         => $handler_sort,
          ),
          'filter'    => array(
            'handler'         => $handler_filter,
          ),
        );
      }
    }
  }

  return $data;
}
