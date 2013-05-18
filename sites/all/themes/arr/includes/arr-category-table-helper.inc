<?php

/**
 * Helper class to deal with category pages
 */
class arr_vendor_category_page {
  var $ratings = array();
  var $vtype = 'vendor';
  var $print_nodes = array();
  var $table;
  
  /**
   * Construct function
   */
  function __construct($vendor_type = '') {
    if (!empty($vendor_type)) {
      $this->vtype = $vendor_type;
    } else {
      global $_arr_site;
      $this->vtype = !empty($_arr_site['vendor_type']) ? $_arr_site['vendor_type'] : 'vendor';
    }
    
    $this->table = new arr_vendor_category_table();
  }
  
  /**
   * Builds the ratings
   */
  function build_ratings(&$nodes) {
    foreach ($nodes as $key => $node) {
      if (empty($node['#node']->type)) continue;
      if ($node['#node']->type != $this->vtype) continue;
      
      $n = $node['#node'];
      $item = '';
      
      $field = field_view_field('node', $node['#node'], 'field_arr_rating', 'default');
      
      if (!empty($field['#items'])) {
        $item = $field['#items'][0]['value'];
      }
      
      if (is_numeric($item)) {
        if (!isset($this->ratings[$item])) $this->ratings[$item] = array();
        
        $this->ratings[$item][] = $node;
        
        unset($nodes[$key]);
      }
    }
    
    $this->sort_nodes_by_ratings();
  }
  
  /**
   * Reorganizes the nodes array in order of ratings
   */
  function sort_nodes_by_ratings() {
    if (ksort($this->ratings)) {
      $nodes_updated = array_reverse($this->ratings, TRUE);
      
      foreach ($nodes_updated as $rating => $node_group) {
        foreach ($node_group as $node) {
          if (!empty($node['#node'])) $this->print_nodes[] = $node;
        }
      }
    }
  }
}

/**
 * Helper class for category rows
 */
class arr_vendor_category_table {
  var $thead = array();
  var $tbody = array();
  
  /**
   * Construct function
   */
  function __construct() {
  }
  
  /**
   * Adds a row to the table
   */
  function add_row($row_key, $attributes = array(), $outer_key = 'tbody') {
    $this->{$outer_key}[$row_key] = new arr_vendor_category_table_row($attributes);
  }
}

/**
 * Helper class for a category table row
 */
class arr_vendor_category_table_row {
  var $cells = array();
  var $attributes = array();
  
  function __construct($attr) {
    $this->attributes = $attr;
  }
  
  function add_cell($content = '', $attributes = array()) {
    $this->cells[] = new arr_vendor_category_table_cell($content, $attributes);
  }
}

/**
 * Helper class for a category table cell
 */
class arr_vendor_category_table_cell {
  function __construct($content = '', $attributes = array()) {
    $this->content = $content;
    $this->attributes = $attributes;
  }
}
