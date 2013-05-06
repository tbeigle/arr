<?php

/**
 * Add body classes if certain regions have content.
 *
 * Add IE-specific elements if necessary.
 */
function arr_preprocess_html(&$vars) {
  global $theme_path, $user, $_arr_site;
  
  if (theme_get_setting('arr_enable_home_ss') == 1) {
    $vars['classes_array'][] = 'arr-show-home-ss';
  }
  
  if (!empty($vars['arr_site_term_specs'])) {
    dpm($vars['arr_site_term_specs']);
  }
  $args = array(
    arg(0),
    arg(1),
    arg(2),
  );
  
  $is_category_page = ($args[0] == 'taxonomy' && $args[1] == 'term' && $args[2] == 1);
  
  if (!empty($_arr_site['is_category_page'])) {
    $vars['classes_array'][] = 'vendor-categories-page';
  }
  
  if (empty($theme_path)) $theme_path = 'sites/all/themes/arr';
  
  if (!empty($vars['page']['content_top'])) {
    $vars['classes_array'][] = 'with-content-top';
  }
  
  if (!empty($vars['page']['right_col'])) {
    $vars['classes_array'][] = 'with-right-col';
  }
  
  // Set the $ie variable
  $ie = $vars['ie'] = arr_is_ie();
  $html_classes = array();
  $vars['ie_meta'] = '';
  
  // Set the HTML classes and add the IE meta tag
  if (!empty($ie)) {
    $ie_meta = array(
      '#type' => 'html_tag',
      '#tag' => 'meta',
      '#attributes' => array(
        'http-equiv' => 'X-UA-Compatible',
        'content' =>  'IE=edge',
      ),
      '#weight' => '-99999',
    );
    
    drupal_add_html_head($ie_meta, 'meta_ie_render_engine');
    
    $html_classes[] = 'lte9';
    
    if ($ie == 9) {
      $html_classes[] = 'ie9';
    } elseif ($ie <= 8) {
      $html_classes[] = 'lte8';
      $html_classes[] = ($ie == 8) ? 'ie8' : 'ie7';
    }
  }
  
  if ($vars['is_front']) {
    $html_classes[] = 'front-bg';
  }
  
  $vars['html_classes'] = !empty($html_classes) ? ' '.implode(' ', $html_classes) : '';
  
  // Add the viewport meta tag
  $viewport_tag = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'viewport',
      'content' => 'width=device-width',
    ),
    '#weight' => '-88888',
  );
  
  drupal_add_html_head($viewport_tag, 'meta_viewport');
  
  // Add the Apple Touch icons
  $apple_icons = array(144, 114, 72, 57);
  
  $icons_path = '/'.$theme_path.'/images/icons/apple-touch-icon-||DIM||.png';
  
  foreach ($apple_icons as $size) {
    $dim = $size.'x'.$size;
    
    $apple_icon = array(
      '#tag' => 'link',
      '#attributes' => array(
        'rel' => 'apple-touch-icon',
        'sizes' => $dim,
        'href' => str_replace('||DIM||', $dim, $icons_path),
      ),
    );
    
    drupal_add_html_head($apple_icon, 'apple_touch_icon_'.$dim);
  }
  
  /**
   * Adding classes to the body
   */
  if (isset($vars['classes_array'])) {
    if (arr_is_array($user->roles)) {
      foreach ($user->roles as $r) {
        $vars['classes_array'][] = 'role-'.strtolower(preg_replace('/[^A-Za-z0-9\-]/', '-', $r));
      }
    }
  }
}

/**
 * Override or insert variables into the page template for HTML output.
 */
function arr_process_html(&$vars) {
}

/**
 * Preprocess the page
 */
function arr_preprocess_page(&$vars) {
  global $_arr_site;
  
  $vendor_type = !empty($_arr_site['vendor_type']) ? $_arr_site['vendor_type'] : 'vendor';
  
  $vars['is_vendor_page'] = FALSE;
  $vars['schema_main_content'] = '';
  $vars['schema_vendor_name'] = '';
  $vars['schema_vendor_desc'] = '';
  
  $nodes = !empty($vars['page']['content']['system_main']['nodes']) ? $vars['page']['content']['system_main']['nodes'] : array();
  $node = NULL;
  
  if (count($nodes == 1) && !empty($vars['node'])) {
    $node = $vars['node'];
  }
  
  if (!empty($node->type)) {
    if ($node->type == $vendor_type) {
      $vars['is_vendor_page'] = TRUE;
      $vars['schema_main_content'] = ' itemscope itemtype="http://schema.org/Organization"';
      $vars['schema_vendor_name'] = ' itemprop="name"';
      $vars['schema_vendor_desc'] = ' itemprop="description"';
      
      $vendor_categories = field_view_field('node', $node, 'field_vendor_categories', 'default');
      $vendor_categories['#theme'] = 'tax_title_prefix';
      $vars['vendor_category'] = render($vendor_categories);
      
      $field_description = field_view_field('node', $node, 'field_description', 'default');
      $vars['description'] = render($field_description);
    }
  }
  
  // Vendor categories pages
  if (!empty($_arr_site['is_category_page'])) {
    // $vars['page']['content']['system_main']['nodes'] is an array of full node objects
    $print_nodes = array();
    
    $nodes = !empty($vars['page']['content']['system_main']['nodes']) ? $vars['page']['content']['system_main']['nodes'] : array();
    $ratings = array();
    
    // Reorganize the nodes array in order of ratings
    foreach ($nodes as $key => $node) {
      $type = !empty($node['#node']->type) ? $node['#node']->type : '';
      
      if (!empty($node['#node']->nid) && $type == $vendor_type) {
        $n = $node['#node'];
        $item = '';
        
        $field = field_view_field('node', $n, 'field_arr_rating', 'default');
        
        if (!empty($field['#items'])) {
          $item = $field['#items'][0]['value'];
        }
        
        if (is_numeric($item)) {
          if (!isset($ratings[$item])) $ratings[$item] = array();
          
          $ratings[$item][] = $node;
          
          unset($vars['page']['content']['system_main']['nodes'][$key]);
        }
      }
    }
    
    if (ksort($ratings)) {
      $nodes_updated = array_reverse($ratings, true);
      
      foreach ($nodes_updated as $rating => $node_group) {
        foreach ($node_group as $node) {
          if (!empty($node['#node'])) $print_nodes[] = $node;
        }
      }
    }
    
    $category_rows = array(
      'thead' => array(
        'top_row' => array(
          '#attributes' => array(
            'class' => array(
              'top-row',
            ),
          ),
          'cells' => array(
            array(
              'content' => t('Vendors'),
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
      ),
      'tbody' => array(
        'arr_rating' => array(
          '#attributes' => array(
            'class' => array(
              'ratings',
              'arr-rating',
            ),
          ),
          'cells' => array(
            array(
              'content' => t('ARR Rating'),
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
        'user_rating' => array(
          '#attributes' => array(
            'class' => array(
              'ratings',
              'user-rating',
            ),
          ),
          'cells' => array(
            array(
              'content' => t('Customer Rating'),
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
        'links' => array(
          '#attributes' => array(
            'class' => array(
              'links',
            ),
          ),
          'cells' => array(
            array(
              'content' => '',
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
        'bottom_line_button' => array(
          '#attributes' => array(
            'class' => array(
              'bottom-line-button-row',
              'collapsible-button-row',
            ),
          ),
          'cells' => array(
            array(
              'content' => 
                '<a class="collapse-expand-link" data-collapse-target=".bottom-line" href="#">'.
                  t('Bottom Line').
                '</a>',
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
        'bottom_line' => array(
          '#attributes' => array(
            'class' => array(
              'bottom-line',
              'collapsible',
              'expanded',
            ),
          ),
          'cells' => array(
            array(
              'content' => '',
              '#attributes' => array(
                'class' => array(
                  'key-column',
                ),
              ),
            ),
          ),
        ),
      ),
    );
    
    if (!empty($_arr_site['arr_site_cat_specs']) && !empty($_arr_site['arr_site_node_specs'])) {
      $cat_specs = $_arr_site['arr_site_cat_specs'];
      $node_specs = $_arr_site['arr_site_node_specs'];
      $n_specs = array();
      
      if (!empty($node_specs['spec_categories'])) {
        if (is_array($node_specs['spec_categories'])) {
          $cats_in_use = $node_specs['spec_categories'];
        }
      }
      
      if (!empty($node_specs['specs'])) {
        if (is_array($node_specs['specs'])) {
          $specs_in_use = $node_specs['specs'];
        }
      }
      
      if (!empty($node_specs['nodes'])) {
        if (is_array($node_specs['nodes'])) {
          $n_specs = $node_specs['nodes'];
        }
      }
      
      if (!empty($cats_in_use) && !empty($specs_in_use)) {
        foreach ($cat_specs as $spec_cat_id => $spec_cat_data) {
          if (!empty($spec_cat_data['cat']) && !empty($spec_cat_data['rows']) && in_array($spec_cat_id, $cats_in_use)) {
            $key_class = 'collapsible-spec-cat-'.$spec_cat_id;
            
            $category_rows['tbody']['spec_cat_'.$spec_cat_id] = array(
              '#attributes' => array(
                'class' => array(
                  'cat-row',
                  'cat-row-'.$spec_cat_id,
                  'collapsible-button-row',
                  'ratings',
                ),
              ),
              'cells' => array(
                array(
                  'content' =>
                    '<a class="collapse-expand-link" data-collapse-target=".'.$key_class.'" href="#">'.
                      t('!cat', array('!cat' => $spec_cat_data['cat'])).
                    '</a>',
                  '#attributes' => array(
                    'class' => array(
                      'key-column',
                      'cat-start',
                    ),
                  ),
                ),
              ),
            );
            
            $i = 0;
            $last_spec = $pen_ult_spec = '';
            
            foreach ($spec_cat_data['rows'] as $spec_id => $spec) {
              if (in_array($spec_id, $specs_in_use)) {
                $pen_ult_spec = $last_spec;
                $last_spec = 'spec_'.$spec_id;
                
                $category_rows['tbody']['spec_'.$spec_id] = array(
                  '#attributes' => array(
                    'class' => array(
                      $key_class,
                      'collapsible',
                      'spec-row',
                      'collapsed',
                    ),
                  ),
                  'cells' => array(
                    array(
                      'content' => t('!spec', array('!spec' => $spec)),
                      '#attributes' => array(
                        'class' => array(
                          'key-column',
                          'spec-column',
                        ),
                      ),
                    ),
                  ),
                );
                
                if ($i == 0) {
                  $category_rows['tbody']['spec_'.$spec_id]['#attributes']['class'][] = 'first-cat-spec-row';
                  $i++;
                }
              }
            }
            if (!empty($last_spec) && !empty($pen_ult_spec)) {
              $category_rows['tbody'][$pen_ult_spec]['#attributes']['class'][] = 'pen-ult-spec';
              $category_rows['tbody'][$last_spec]['#attributes']['class'][] = 'last-spec';
            }
          }
        }
      }
      
      foreach ($print_nodes as $node_array) {
        $node = $node_array['#node'];
        $website = !empty($node_array['field_vendor_website'][0]['#markup']) ? $node_array['field_vendor_website'][0]['#markup'] : '';
        $nid = $node->nid;
        $node_path = 'node/'.$nid;
        
        // head
        $content = $text = '';
        
        $options = array(
          'attributes' => array(
            'class' => array(
              'vendor-link',
            ),
          ),
          'html' => TRUE,
        );
        
        $lowest_cost = '';
        
        if (!empty($node->field_logo['und'][0]['uri'])) {
          $options['attributes']['class'][] = 'vendor-logo';
          $img_url = image_style_url('vendor_category_page_logos', $node->field_logo['und'][0]['uri']);
          
          if (!empty($node_array['field_lowest_cost']['#items'][0]['value'])) {
            $lowest_cost = arr_vendor_lowest_cost(array('element' => $node_array['field_lowest_cost']));
            
            $_arr_site['has_lowest_cost'] = TRUE;
          }
          
          $text = '<img src="'.$img_url.'" class="vendor-category-logo" alt="'.$node->title.'">';
        } else {
          $options['attributes']['class'][] = 'vendor-text';
          $text = '<span class="title-text">'.$node->title.'</span>';
        }
        
        $category_rows['thead']['top_row']['cells'][] = array('content' => $lowest_cost.l($text, $node_path, $options));
        
        // arr_rating
        $content = '';
        
        if (!empty($node->field_arr_rating['und'][0]['value'])) {
          $field = field_view_field('node', $node, 'field_arr_rating');
          $field['is_category_page'] = TRUE;
          
          $content .= theme('vendor_star_rating', $field);
        }
        
        $category_rows['tbody']['arr_rating']['cells'][] = array(
          'content' => $content,
        );
        
        // user_rating
        $ratings = array();
        
        if (db_table_exists('field_data_field_rating')) {
          $sql =
            'SELECT f.`field_rating_value` AS `field_rating_value` '.
            'FROM {field_data_field_rating} f '.
              'LEFT JOIN {comment} c '.
                'ON f.`entity_id` = c.`cid` '.
            'WHERE f.`entity_type` = :entity_type '.
              'AND c.`status` = :status '.
              'AND c.`nid` = :nid '.
            'ORDER BY c.`nid`, c.`status` DESC;';
          $results = db_query($sql, array(':entity_type' => 'comment', ':status' => 1, ':nid' => $nid));
          
          foreach ($results as $row) {
            if (!empty($row->field_rating_value)) {
              if (is_numeric($row->field_rating_value)) {
                $ratings[] = $row->field_rating_value;
              }
            }
          }
        }
        
        $category_rows['tbody']['user_rating']['cells'][] = array(
          'content' => theme('vendor_star_rating', array('ratings' => $ratings, 'is_user' => TRUE)),
        );
        
        // links
        $vendor_site_link = l(t('Vendor Website'), $website, array('attributes' => array('class' => array('vendor-website'))));
        $full_review_link = l(t('Full Vendor Review'), $node_path, array('attributes' => array('class' => array('full-review'))));
        
        $category_rows['tbody']['links']['cells'][] = array(
          'content' => 
            '<p class="vendor-link">'.$vendor_site_link.'</p>'.
            '<p class="full-review">'.$full_review_link.'</p>',
        );
        
        // bottom_line_button
        $category_rows['tbody']['bottom_line_button']['cells'][] = array('content' => '');
        
        // bottom_line
        $bottom_line = !empty($node->field_bottom_line['und'][0]['safe_value']) ? $node->field_bottom_line['und'][0]['safe_value'] : '';
        $category_rows['tbody']['bottom_line']['cells'][] = array(
          'content' => $bottom_line,
        );
        
        // spec_cat_%
        foreach ($cats_in_use as $spec_cat_id) {
          $content = '';
          
          if (!empty($n_specs[$nid][$spec_cat_id]['rating'])) {
            $content .= theme('vendor_star_rating', array('cat_rating' => $n_specs[$nid][$spec_cat_id]['rating'], 'is_category_page' => TRUE));
          }
          
          $category_rows['tbody']['spec_cat_'.$spec_cat_id]['cells'][] = array(
            'content' => $content,
          );
        }
        
        foreach ($specs_in_use as $spec_id) {
          $content = '<p class="spec-info">N/A</p>';
          
          if (!empty($n_specs[$nid])) {
            foreach ($n_specs[$nid] as $spec_cat_id => $data) {
              if (!empty($data['specs'][$spec_id])) {
                $spec = is_array($data['specs'][$spec_id]) ? $data['specs'][$spec_id] : array();
                
                if (!empty($spec['spec_comment'])) {
                  $content = '<p class="spec-info">'.$spec['spec_comment'].'</p>';
                } elseif (isset($spec['has_spec'])) {
                  $spec_info = ($spec['has_spec'] == 1) ? 'YES' : 'NO';
                  $content = '<p class="spec-info">'.$spec_info.'</p>';
                }
                break;
              }
            }
          }
          
          $category_rows['tbody']['spec_'.$spec_id]['cells'][] = array(
            'content' => $content,
          );
        }
      }
      
      if (!empty($category_rows)) {
        $vars['page']['content']['vendors_table'] = array(
          'rows' => $category_rows,
          '#theme' => 'vendor_category_table',
        );
      }
    }
  }
}

/**
 * Override or insert variables into the page template.
 */
function arr_process_page(&$vars) {
}

/**
 * Implements hook_preprocess_maintenance_page().
 */
function arr_preprocess_maintenance_page(&$vars) {
  // By default, site_name is set to Drupal if no db connection is available
  // or during site installation. Setting site_name to an empty string makes
  // the site and update pages look cleaner.
  // @see template_preprocess_maintenance_page
  if (!$vars['db_is_active']) {
    $vars['site_name'] = '';
  }
  
  drupal_add_css(drupal_get_path('theme', 'bartik') . '/css/maintenance-page.css');
}

/**
 * Override or insert variables into the maintenance page template.
 */
function arr_process_maintenance_page(&$vars) {
}

/**
 * Override or insert variables into the node template.
 */
function arr_preprocess_node(&$vars) {
  global $_arr_site;
  
  $vars['node_edit_link'] = (user_access('administer nodes')) ? '<p><a class="edit-link" href="http://allravereviews.com/node/'.$vars['node']->nid.'/edit?destination=admin/content">edit node</a></p>' : '';
  
  $vendor_type = !empty($_arr_site['vendor_type']) ? $_arr_site['vendor_type'] : 'vendor';
  $vars['is_categories_page'] = $is_category_page = !empty($_arr_site['is_category_page']);
  $vars['schema_node'] = '';
  $vars['schema_review'] = '';
  
  if ($type = $vars['type']) {
    if ($type == $vendor_type) {
      if ($vars['view_mode'] == 'full' && node_is_page($vars['node'])) {
        $vars['classes_array'][] = 'node-full';
        $vars['classes_array'][] = 'clearfix';
        
        $vars['schema_node'] = ' itemprop="review" itemscope itemtype="http://schema.org/Review"';
        $vars['schema_review'] = ' itemprop="description"';
      } elseif ($is_category_page) {        
        $classes = array(
          'column',
          'left',
        );
        
        foreach ($classes as $class) {
          if (!in_array($class, $vars['classes_array'])) {
            $vars['classes_array'][] = $class;
          }
        }
      }
      
      $user_ratings = array();
      
      if (!empty($vars['content']['comments']['comments'])) {
        foreach ($vars['content']['comments']['comments'] as $k => $c) {
          if (is_numeric($k) && is_array($c)) {
            if (!empty($c['field_rating']['#items'])) {
              $item = array_pop($c['field_rating']['#items']);
              $val = !empty($item['value']) ? $item['value'] : '';
              $rating = is_numeric($val) ? $val : -1;
              
              if (isset($rating) && $rating <= 5 && $rating >= 0) {
                $user_ratings[] = $rating;
              }
            }
          }
        }
        
        if (!empty($user_ratings)) {
          $vars['content']['field_user_rating']['ratings'] = $user_ratings;
          $vars['content']['field_user_rating']['is_user'] = TRUE;
        }
      }
      
      $vars['vendor_top_classes'] = array (
        'wrapper' => 'vendor-top row clearfix',
        'logo' => 'vendor-logo',
        'arr_rating' => 'arr-rating',
        'contact' => 'contact-vendor',
      );
      
      $vars['hide_fields'] = $hide_fields = array(
        'field_lowest_cost',
        'field_vendor_website',
        'field_logo',
        'field_arr_rating',
        'field_description',
        'field_feature_vendor_on_the_home',
        'field_arr_rating',
        'field_vendor_categories',
      );
      
      if (!empty($vars['content']['field_user_rating'])) {
        $vars['hide_fields'][] = 'field_user_rating';
      }
      
      if ($is_category_page && !empty($vars['content']['field_bottom_line'])) {
        $vars['hide_fields'][] = 'field_bottom_line';
      }
      
      $theme_fields = array(
        'lowest_cost' => 'vendor_lowest_cost',
        'vendor_website' => 'vendor_href',
        'logo' => 'vendor_logo',
        'arr_rating' => 'vendor_star_rating',
        'user_rating' => 'vendor_star_rating',
      );
      
      if ($is_category_page) {
        unset($theme_fields['logo']);
      }
      
      foreach ($theme_fields as $field => $theme) {
        if (!empty($vars['content']['field_'.$field])) {
          $vars['content']['field_'.$field]['#theme'] = $theme;
        } else {
          $vars['content']['field_'.$field] = FALSE;
        }
      }
      
      if (!empty($vars['content']['field_lowest_cost'])) {
        $rendered = render($vars['content']['field_lowest_cost']);
        
        if (!empty($rendered) && $rendered != ' ') {
          $vars['vendor_top_classes']['wrapper'] .= ' lowest-cost';
        }
      }
    }
  }
}

/**
 * Implements template_preprocess_taxonomy_term()
 */
function arr_preprocess_taxonomy_term(&$vars) {
  global $_arr_site;
  $name = !empty($vars['vocabulary_machine_name']) ? $vars['vocabulary_machine_name'] : '';
  $page = !empty($vars['page']) ? $vars['page'] : FALSE;
  
  if (!empty($_arr_site['is_category_page'])) {
  }
}

/**
 * Template preprocessor for the comment wrapper template
 */
function arr_preprocess_comment_wrapper(&$vars) {
  global $_arr_site;
  
  $vars['content']['field_user_rating'] = FALSE;
  
  if (empty($_arr_site['is_vendor_page'])) return;
  
  $comments = !empty($vars['content']['comments']) ? $vars['content']['comments'] : array();
  
  $user_ratings = array();
  
  foreach ($comments as $k => $c) {
    if (is_numeric($k) && is_array($c)) {
      if (!empty($c['field_rating']['#items'])) {
        $item = array_pop($c['field_rating']['#items']);
        $val = !empty($item['value']) ? $item['value'] : '';
        $rating = is_numeric($val) ? $val : -1;
        
        if (isset($rating) && $rating <= 5 && $rating >= 0) {
          $user_ratings[] = $rating;
        }
      }
    }
  }
  
  if (!empty($user_ratings)) {
    $vars['content']['field_user_rating']['ratings'] = $user_ratings;
    $vars['content']['field_user_rating']['is_user'] = TRUE;
    $vars['content']['field_user_rating']['is_comment_rating'] = TRUE;
    $vars['content']['field_user_rating']['#theme'] = 'vendor_star_rating';
  }
}

/**
 * Template preprocessor for the comment template
 */
function arr_preprocess_comment(&$vars) {
  if (!empty($vars['content']['field_rating'])) {
    $vars['content']['field_rating']['is_user'] = TRUE;
    $vars['content']['field_rating']['#theme'] = 'vendor_star_rating';
  }
}

/**
 * Override or insert variables into the block template.
 */
function arr_preprocess_block(&$vars) {
  // In the header region visually hide block titles.
  if ($vars['block']->region == 'header') {
    $vars['title_attributes_array']['class'][] = 'element-invisible';
  } elseif ($vars['block']->region == 'content_bottom') {
    //dpm($vars['block']);
  }
}

/********************
ARR Themes
********************/
/**
 * Implements hook_theme()
 */
function arr_theme($existing, $type, $theme, $path) {
  return array(
    'tax_title_prefix' => array(
      'render element' => 'element',
    ),
    'vendor_href' => array(
      'render element' => 'element',
    ),
    'vendor_logo' => array(
      'render element' => 'element',
    ),
    'vendor_lowest_cost' => array(
      'render element' => 'element',
    ),
    'vendor_star_rating' => array(
      'render element' => 'element',
    ),
    'vendor_category_table' => array(
      'render element' => 'rows',
    ),
  );
}

/**
 * Function to theme the vendor category tables
 */
function arr_vendor_category_table($vars) {
  global $_arr_site;
  
  $output = '';
  
  $el = $vars['rows'];
  $rows = !empty($el['rows']) ? $el['rows'] : '';
  $inner_class = 'inner'.(!empty($_arr_site['has_lowest_cost']) ? ' has-lowest-cost' : '');
  
  if (!empty($rows)) {
    $output .=
      '<div id="vendors-by-category-wrapper">'.
        '<div class="'.$inner_class.'">'.
          '<table id="vendors-by-category">';
    
    foreach ($rows as $section_tag => $rows) {
      $cell_tag = ($section_tag == 'thead') ? 'th' : 'td';
      
      $output .= '<'.$section_tag.'>';
      
      foreach ($rows as $row_key => $row) {
        $row_class = !empty($row['#attributes']['class']) ? $row['#attributes']['class'] : array();
        $is_spec = in_array('spec-row', $row_class);
        
        $row_attr = !empty($row['#attributes']) ? arr_attr($row['#attributes']) : '';
        $output .= '<tr'.$row_attr.'>';
        
        $add_first_ven_class = $no_top_b_class = FALSE;
        
        foreach ($row['cells'] as $cell_key => $cell) {
          if (empty($cell['#attributes'])) $cell['#attributes'] = array();
          if (empty($cell['#attributes']['class'])) $cell['#attributes']['class'] = array();
          
          $is_key = in_array('key-column', $cell['#attributes']['class']);
          $is_ven = in_array('vendor-column', $cell['#attributes']['class']);
          
          if (!$is_key && !$is_ven) {
            $cell['#attributes']['class'][] = 'vendor-column';
            
            if ($add_first_ven_class) {
              $cell['#attributes']['class'][] = 'first-vendor';
              
              $add_first_ven_class = FALSE;
            }
          } elseif ($is_key) {
            $add_first_ven_class = TRUE;
          }
          
          $cell_attr = arr_attr($cell['#attributes']);
          $output .= '<'.$cell_tag.$cell_attr.'><div class="cell-wrapper">'.$cell['content'].'</div></'.$cell_tag.'>';
        }
        
        $output .= '</tr>';
      }      
      
      $output .= '</'.$section_tag.'>';
    }
    
    $output .=
          '</table>'.
        '</div>'.
      '</div>';
  }
  
  return $output;
}

function arr_attr($attributes) {
  $output = '';
  
  foreach ($attributes as $key => $val) {
    $v = is_array($val) ? implode(' ', $val) : $val;
    $output .= ' '.$key.'="'.$v.'"';
  }
  
  return $output;
}

/**
 * Function to theme taxonomy terms to be used in the title on a vendor page
 */
function arr_tax_title_prefix($element) {
  $output = '';
  
  if (!empty($element['element'])) {
    $el = $element['element'];
    
    $first_term = !empty($el['#items']) ? array_shift($el['#items']) : NULL;
    
    if (!empty($first_term['taxonomy_term'])) {
      $output = $first_term['taxonomy_term']->name;
    }
  }
  
  return $output;
}

/**
 * Function to theme the vendor website URL as actual plain text (without any div wrappers)
 */
function arr_vendor_href($element) {
  $output = '';
  
  $el = !empty($element['element']) ? $element['element'] : array();
  $item = !empty($el[0]) ? $el[0] : array();
  $output = !empty($item['#markup']) ? ' href="'.$item['#markup'].'" target="_blank"' : '';
  
  return $output;
}

/**
 * Function to theme the vendor logo
 */
function arr_vendor_logo($element) {
  $output = '';
  
  $el = !empty($element['element']) ? $element['element'] : array();
  $items = !empty($el['#items']) ? $el['#items'] : array();
  $item = !empty($items[0]) ? $items[0] : array();
  $uri = !empty($item['uri']) ? $item['uri'] : NULL;
  
  $title = !empty($el['#object']->title) ? $el['#object']->title : '';
  
  if (!empty($uri)) $output = '<img src="'.image_style_url('vendor_full', $uri).'" alt="logo - '.$title.'">';
  
  return $output;
}

/**
 * Function to theme the lowest cost bubble
 */
function arr_vendor_lowest_cost($element) {
  $output = ' ';
  
  $el = !empty($element['element']) ? $element['element'] : array();
  
  if (!empty($el['#items'][0]['value'])) {
    $output =
      '<div class="lowest-cost-bubble">'.
        '<strong class="inner">Lowest Cost</strong>'.
      '</div>'.
      '<!-- /.lowest-cost-bubble -->';
  }
  
  return $output;
}

/**
 * Function to theme rating stars
 */
function arr_vendor_star_rating($vars) {
  $output = '';
  
  $el = !empty($vars['element']) ? $vars['element'] : NULL;
  $is_agg = FALSE;
  $is_user = (!empty($el['is_user']) || !empty($vars['is_user']));
  $is_cat = (!empty($el['is_category_page']) || !empty($vars['is_category_page']));
  $is_com = !empty($el['is_comment_rating']);
  $is_cat_rating = (!empty($el['cat_rating']) || !empty($vars['cat_rating']));
  $has_ratings = (!empty($el['ratings']) || !empty($vars['ratings']));
  
  if (!empty($el) || $is_cat_rating || $has_ratings) {
    if (!empty($el['cat_rating']) || !empty($vars['cat_rating'])) {
      $single_rating = !empty($el['cat_rating']) ? $el['cat_rating'] : $vars['cat_rating'];
    } elseif (!empty($el['#items'][0]['value'])) {
      $single_rating = $el['#items'][0]['value'];
    } elseif (!empty($el['ratings']) || !empty($vars['ratings'])) {
      $ratings = !empty($el['ratings']) ? $el['ratings'] : $vars['ratings']; 
      if (is_array($ratings)) {
        $agg_data = _arr_breakdown_aggregate($ratings);
        $is_agg = TRUE;
      }
    }
  }
  
  if (!empty($single_rating) || (!empty($agg_data['count']) && !empty($agg_data['aggregate']))) {
    $worst = $best = $review_count = $text = '';
    
    if ($is_agg) {
      $rating = $agg_data['aggregate'];
      $review_count = ' out of 5 stars based on <span itemprop="reviewCount">'.$agg_data['count'].'</span> user reviews';
      $itemprop = 'aggregateRating';
      $itemtype = 'http://schema.org/AggregateRating';
      
      if (!$is_cat && $is_user) {
        $text = t('User Rating | Based on !count Reviews', array('!count' => $agg_data['count']));
      }
    } else {
      $rating = $single_rating;
      $worst = '<meta itemprop="worstRating" content="1" />';
      $best = ' out of <span itemprop="bestRating">5</span> stars';
      $itemprop = 'reviewRating';
      $itemtype = 'http://schema.org/Rating';
      
      if (!$is_cat && !$is_user) {
        $text = t('ARR Rating');
      }
    }
    
    if (!$is_cat && !$is_com) {
      $output .=
        '<div class="hide-me" itemprop="'.$itemprop.'" itemscope itemtype="'.$itemtype.'">'.
          $worst.
          'Rated <span itemprop="ratingValue">'.$rating.'</span> '.
          $best.
          $review_count.
        '</div>';
    }
    
    $extra_class = ($is_user) ? ' user-rating' : ' arr-rating';
    
    $output .= _arr_site_star_rating_ul($rating, $extra_class, $text);
  } else {
    $output .= '<p class="no-ratings">'.t('No rating(s) available.').'</p>';
  }
  
  return $output;
}

/**
 * Helper function to get the aggregate rating data
 */
function _arr_breakdown_aggregate($data) {
  $output = array(
    'count' => 0,
    'aggregate' => 0,
  );
  
  $total = 0;
  
  foreach ($data as $rating) {
    if (is_numeric($rating)) {
      $total += $rating;
      $output['count']++;
    }
  }
  
  if (!empty($output['count']) && isset($total)) {
    $output['aggregate'] = round($total / $output['count']);
  }
  
  return $output;
}

/**
 * Builds the star rating ul
 */
function _arr_site_star_rating_ul($rating, $extra_class = '', $text = '') {
  $output = '<ul class="stars clearfix'.(!empty($extra_class) ? ' '.$extra_class : '').'">';
  
  for ($i = 1; $i < 6; $i++) {
    $rating_class = array(
      'left',
      'star',
      'star-'.$i,
    );
    
    if ($i <= $rating) {
      $rating_class[] = 'full';
    } else {
      $rating_class[] = 'empty';
    }
    
    $output .= '<li class="'.implode(' ', $rating_class).'"></li>';
  }
  
  $output .= !empty($text) ? '<li class="left text">'.$text.'</li>' : '';
  
  $output .= '</ul>';
  
  return $output;
}

/***********************
Drupal theme hooks
***********************/
/**
 * Implementation of theme_breadcrumb()
 */
function arr_breadcrumb($vars) {
  global $_arr_site;
  
  $output = '';
  
  $bc = $vars['breadcrumb'];
  
  if (!empty($bc)) {
    $output = '<ul class="breadcrumb">';
    
    $i = 0;
    
    foreach ($bc as $v) {
      if (strpos($v, 'no-link') === FALSE) {
        $pre = ($i > 0) ? '<span class="sep">/</span>': '';
        
        $output .= '<li>'.$pre.$v.'</li>';
        
        $i++;
      }
    }
    
    if (!empty($_arr_site['breadcrumb_end'])) {
      $output .= '<li class="bc-cur"><span class="sep">/</span>'.$_arr_site['breadcrumb_end'].'</li>';
    }
    
    $output .= '</ul>';
  }
  
  return $output;
}

/**
 * Theme override for the comment form
 */
function arr_form_comment_form_alter(&$form, &$form_state) {
  $form['#attributes']['class'][] = 'lte8-label-show';
  
  $form['field_rating']['und']['#title_display'] = 'invisible';
  
  if (!empty($form['author']['name']['#title'])) {
    $form['author']['name']['#attributes']['placeholder'] = $form['author']['name']['#title'];
    $form['author']['name']['#attributes']['style'] = 'display: none;';
  }
  
  $form['subject']['#title'] = t('Review Headline');
  $form['subject']['#attributes']['placeholder'] = 'Review Headline';
  
  $form['actions']['submit']['#value'] = 'Submit Review';
  $form['actions']['submit']['#attributes']['class'][] = 'comment-submit';
}

/**
 * Implements theme_menu_tree().
 */
function arr_menu_tree($vars) {
  return '<ul class="menu clearfix">' . $vars['tree'] . '</ul>';
}

/**
 * Implements theme_menu_link()
 */
function arr_menu_link(array $vars) {
  global $_arr_link_counts;
  
  $el = $vars['element'];
  $menu_name = !empty($el['#original_link']['menu_name']) ? $el['#original_link']['menu_name'] : '';
  $sub_menu = '';

  if ($el['#below']) {
    $sub_menu = drupal_render($el['#below']);
  }
  
  $output = l($el['#title'], $el['#href'], $el['#localized_options']);
  
  if (isset($el['#localized_options']['attributes']['id'])) {
    $el['#attributes']['class'][] = $el['#localized_options']['attributes']['id'];
  }
  
  if (empty($_arr_link_counts)) $_arr_link_counts = array();
  
  if (!empty($menu_name)) {
    if (!isset($_arr_link_counts[$menu_name])) $_arr_link_counts[$menu_name] = 0;
    
    if (($_arr_link_counts[$menu_name] % 2) == 0) {
      $el['#attributes']['class'][] = 'even';
    } else {
      $el['#attributes']['class'][] = 'odd';
    }
    
    $_arr_link_counts[$menu_name]++;
  }
  
  return '<li' . drupal_attributes($el['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Implements theme_field__field_type().
 */
function arr_field__taxonomy_term_reference($vars) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$vars['label_hidden']) {
    $output .= '<h3 class="field-label">' . $vars['label'] . ': </h3>';
  }

  // Render the items.
  $output .= ($vars['element']['#label_display'] == 'inline') ? '<ul class="links inline">' : '<ul class="links">';
  foreach ($vars['items'] as $delta => $item) {
    $output .= '<li class="taxonomy-term-reference-' . $delta . '"' . $vars['item_attributes'][$delta] . '>' . drupal_render($item) . '</li>';
  }
  $output .= '</ul>';

  // Render the top-level DIV.
  $output = '<div class="' . $vars['classes'] . (!in_array('clearfix', $vars['classes_array']) ? ' clearfix' : '') . '"' . $vars['attributes'] .'>' . $output . '</div>';

  return $output;
}

/*********************
VIEWS
*********************/
/**
 * Preprocess view
 */
function arr_preprocess_views_view(&$vars) {
}

/**
 * Preprocess view field
 */
function arr_preprocess_views_view_field(&$vars) {
}

/**
 * Preprocess the logo field for the front vendors block
 */
function arr_preprocess_views_view_field__front_vendors__field_logo(&$vars) {
}

/************************
ARR helper functions
************************/
/**
 * Check if user has a role
 */
function arr_user_has_role($role = '') {
  $output = FALSE;
  
  global $user;
  
  if (arr_is_array($user->roles) && !empty($role)) {
    if (in_array($role, $user->roles)) {
      $output = TRUE;
    }
  }
  
  return $output;
}

/**
 * Helper function to modify the is_array function
 */
function arr_is_array($var) {
  $output = FALSE;
  
  if ($array = $var) {
    if (is_array($array)) {
      $output = TRUE;
    }
  }
  
  return $output;
}

/**
 * Function to check whether the browser is IE and return the version
 *
 * Returns 0 if it's not IE
 */
function arr_is_ie() {
  $version = 0;
  
  if (isset($_SERVER['HTTP_USER_AGENT']) && (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== FALSE)) {
    if (preg_match('/MSIE ([7-9])/i', $_SERVER['HTTP_USER_AGENT'], $matches)) {
      if (is_numeric($matches[1])) $version = $matches[1];
    }
  }
  
  return $version;
}
