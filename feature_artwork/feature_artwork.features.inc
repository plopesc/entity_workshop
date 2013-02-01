<?php
/**
 * @file
 * feature_artwork.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function feature_artwork_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function feature_artwork_node_info() {
  $items = array(
    'artwork' => array(
      'name' => t('Escultura'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
