<?php
$view = new view();
$view->name = 'front_vendors';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Front Vendors';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['style_plugin'] = 'list';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['exclude'] = TRUE;
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['label'] = '';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
$handler->display->display_options['fields']['path']['element_label_colon'] = FALSE;
/* Field: Content: Logo */
$handler->display->display_options['fields']['field_logo']['id'] = 'field_logo';
$handler->display->display_options['fields']['field_logo']['table'] = 'field_data_field_logo';
$handler->display->display_options['fields']['field_logo']['field'] = 'field_logo';
$handler->display->display_options['fields']['field_logo']['label'] = '';
$handler->display->display_options['fields']['field_logo']['alter']['text'] = '<a href=""><img src="[field_logo]" alt="[title]"></a>';
$handler->display->display_options['fields']['field_logo']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_logo']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_logo']['settings'] = array(
  'image_style' => '',
  'image_link' => 'content',
);
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'vendor' => 'vendor',
);
/* Filter criterion: Content: Vendor Featured on the Home Page (field_feature_vendor_on_the_home) */
$handler->display->display_options['filters']['field_feature_vendor_on_the_home_value']['id'] = 'field_feature_vendor_on_the_home_value';
$handler->display->display_options['filters']['field_feature_vendor_on_the_home_value']['table'] = 'field_data_field_feature_vendor_on_the_home';
$handler->display->display_options['filters']['field_feature_vendor_on_the_home_value']['field'] = 'field_feature_vendor_on_the_home_value';
$handler->display->display_options['filters']['field_feature_vendor_on_the_home_value']['value'] = array(
  1 => '1',
);

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
