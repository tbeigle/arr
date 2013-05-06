<?php
/**
 * @file
 * arr_vendors.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function arr_vendors_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}

/**
 * Implements hook_views_api().
 */
function arr_vendors_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_image_default_styles().
 */
function arr_vendors_image_default_styles() {
  $styles = array();

  // Exported image style: vendor_full.
  $styles['vendor_full'] = array(
    'name' => 'vendor_full',
    'effects' => array(),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function arr_vendors_node_info() {
  $items = array(
    'vendor' => array(
      'name' => t('Vendor'),
      'base' => 'node_content',
      'description' => t('Use this content type to create a vendor to feature on the site'),
      'has_title' => '1',
      'title_label' => t('Vendor Name'),
      'help' => '',
    ),
  );
  return $items;
}
