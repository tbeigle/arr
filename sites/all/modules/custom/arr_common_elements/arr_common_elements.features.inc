<?php
/**
 * @file
 * arr_common_elements.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function arr_common_elements_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}

/**
 * Implements hook_views_api().
 */
function arr_common_elements_views_api() {
  return array("version" => "3.0");
}
