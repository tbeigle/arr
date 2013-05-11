(function($) {
  $(document).ready(function() {
    var $body = $('body'),
        is_front = $body.hasClass('front'),
        user_is_admin = $body.hasClass('role-administrator'),
        show_ss_for_all = $body.hasClass('arr-show-home-ss'),
        exp_class = 'expanded',
        exp_col_speed = 250,
        exp_col_tags = {
          col: 'data-collapsed-height',
          exp: 'data-expanded-height'
        };
    
    /**
     * Expand/collapse description
     */
    $('.toggle-description').each(function() {
      var $t = $(this),
          $sp = $t.find('span'),
          $targ = $($t.attr('href')),
          $hid_con = $targ.find('.reveal');
      
      $t.bind('click', function() {
        var new_text = '',
            is_expanded = ($t.hasClass(exp_class) && $targ.hasClass(exp_class));
        
        $hid_con.slideToggle(exp_col_speed, function() {
          if (is_expanded) {
            new_height = $targ.attr(exp_col_tags.col);
            new_text = 'Expand';
            
            $('html, body').animate({scrollTop: $('#page-top').offset().top}, exp_col_speed);
          } else {
            new_text = 'Collapse';
          }
          
          $t.toggleClass(exp_class);
          $targ.toggleClass(exp_class);
          $sp.text(new_text);
        });
        
        return false;
      });
    });
    
    $('.field-name-field-rating .form-item.form-type-radio, #edit-submitted-star-rating .form-item.form-type-radio').each(function(index, value) {
      var $this = $(this),
          $label = $this.find('label'),
          label_id = 'js-star-label-'+index;
      
      $label.attr('id', label_id);
      
      $this.addClass('js-star-wrapper').attr('data-star-index', index).attr('data-label-id', '#'+label_id);
    });
    
    var star_hover_click_func = function(star) {
      var $this = $(star),
          $label = $($this.attr('data-label-id')),
          $radio = $('#'+$label.attr('for'));
      
      $radio.attr('checked', 'checked');
      
      toggle_stars($this.attr('data-star-index'));
    }
    
    $('.form-type-radio.js-star-wrapper')
    .bind('mouseover', function() {
      star_hover_click_func(this);
    })
    .bind('click', function() {
      star_hover_click_func(this);
      $('.form-type-radio.js-star-wrapper').unbind('mouseover');
    });
    
    /**
     * Function to toggle stars on the comments form
     */
    function toggle_stars(star_num) {
      $.each($('.js-star-wrapper'), function(index, value) {
        var $this = $(this),
            $label = $($this.attr('data-label-id')),
            has_class = ($label.hasClass('full-star'));
        
        if ((index <= star_num && !has_class) || (index > star_num && has_class)) {
          $label.toggleClass('full-star');
        }
      });
    }
    
    /**
     * Dropdown menus
     */
    $('.no-link')
    .bind('click', function() {
      return false;
    });
    
    $('li.expanded')
    .bind('mouseenter', function() {
      var $ul = $(this).find('ul');
      
      $ul.fadeIn('fast');/*
      if (!$ul.hasClass('animating')) {
        $ul.addClass('animating').fadeIn('fast', function() {
          $ul.removeClass('animating');
        });
      }*/
    })
    .bind('mouseleave', function() {
      var $ul = $(this).find('ul');
      $ul.fadeOut('fast');
      /*if (!$ul.hasClass('animating')) {
        $ul.addClass('animating').fadeOut('fast', function() {
          $ul.removeClass('animating');
        });
      }*/
    });
    
    /**
     * Field collection slideshow
     */
    var slide_count = 0,
        slides_sel = '.field-name-field-slides > .field-items > .field-item',
        slides_fade_time = 250,
        slide_arrows = {
          both: 'slide-arrows',
          left: 'left-arrow',
          right: 'right-arrow'
        }
    
    function toggle_slides(s) {
      var $this = $(s),
          is_left_arrow = $this.hasClass(slide_arrows.left),
          is_right_arrow = $this.hasClass(slide_arrows.right);
      
      if (!$this.hasClass('animating')) {
        $('.' + slide_arrows.both).addClass('animating');
        
        var $cur_slide = $('#' + $(slides_sel + '.active').attr('id')),
            prev_slide = '#slide-field-' + $cur_slide.attr('data-prev'),
            next_slide = '#slide-field-' + $cur_slide.attr('data-next');
        
        var $to_slide = null;
        
        if (is_left_arrow) {
          $to_slide = $(prev_slide);
        } else if (is_right_arrow) {
          $to_slide = $(next_slide);
        }
        
        if ($to_slide != null) {
          $cur_slide.fadeOut(slides_fade_time, function() {
            $cur_slide.removeClass('active');
            $to_slide.addClass('active').fadeIn(slides_fade_time, function() {
              $('.' + slide_arrows.both).removeClass('animating');
            });
          });
        }
      }
    }
    
    if (is_front) {      
      $(slides_sel).each(function() {
        var $t = $(this),
            t_height = $t.outerHeight(true),
            $parent = $t.parent('.field-items'),
            parent_height = $parent.height();
        
        if (t_height > parent_height) {
          $parent.css('height', t_height);
        }
        
        $t
        .attr('id', 'slide-field-' + slide_count)
        .attr('data-num', slide_count)
        .attr('data-prev', slide_count - 1)
        .attr('data-next', slide_count + 1);
        
        ++slide_count;
      });
      
      if (slide_count > 0) {
        var slides_last_num = slide_count - 1;
        
        $(slides_sel + ':first-child').attr('data-prev', slides_last_num).addClass('active');
        $(slides_sel + ':last-child').attr('data-next', 0);
        
        $('.field-name-field-slides').animate({opacity: 1}, slides_fade_time, function() {
          if (slide_count > 1 && (user_is_admin || show_ss_for_all)) {
            $('#page-main')
            .prepend('<a href="#" class="' + slide_arrows.both + ' ' + slide_arrows.left + '" style="display: none">Slide left</a>')
            .append('<a href="#" class="' + slide_arrows.both + ' ' + slide_arrows.right + '" style="display: none">Slide right</a>');
            
            $('.' + slide_arrows.both).fadeIn(slides_fade_time).bind('click', function() {
              toggle_slides($(this));
              
              return false;
            });
          }
        });
      }
    }
    
    /**
     * Forms
     */
    $('#webform-client-form-12 #edit-submitted-are-you-a-vendor-representative-1').bind('change', function() {
      $('#webform-component-vendor-details').slideToggle('fast');
    });
    
    /**
     * Popups
     */
    var win_height = $(window).height();
    $('#popups').height(win_height);
    $('#bubble-link-suggest-vendor').attr('data-popup', '#popups .suggest-a-vendor').addClass('popup-link');
    $('#bubble-link-suggest-category').attr('data-popup', '#popups .suggest-a-category').addClass('popup-link');
    
    // Add the close popup 
    $('#popups .region-popups .block').each(function() {
      var $pop_block = $(this),
          href = 'href="#'+$pop_block.attr('id')+'"';
      
      $pop_block.prepend('<a class="close-popup" '+href+' title="Click here to close this popup.">X</a>');
    });
    
    // Popup link click functionality
    $('.popup-link').bind('click', function() {
      var $a = $(this),
          $target = $($a.attr('data-popup'));
      
      $('#popups').fadeIn('fast', function() {
        $target.css({'display': 'block', 'opacity': 0});
        
        if ($target.height() > win_height) {
          $target.height(win_height - 40).css({'overflow': 'scroll'});
        }
        
        var popup_top = (win_height - $target.height()) / 2;
        
        $target.css({'top': popup_top+'px'});
        $target.animate({'opacity': 1}, 'fast');
      });
            
      return false;
    });
    
    // Close popup X click functionality
    $('.close-popup').bind('click', function() {
      var $target = $($(this).attr('href'));
      
      $target.fadeOut('fast', function() {
        $('#popups').fadeOut('fast');
      });
      
      return false
    });
    
    $('#edit-submitted-vendor').bind('change', function() {
      var $this = $(this)
          vendor_val = $this.val(),
          $other_vendor = $('#webform-component-new-vendor');
      
      if (vendor_val == 'other' && !$other_vendor.is(':visible')) {
        $other_vendor.slideDown('fast');
      } else if (vendor_val != 'other' && $other_vendor.is(':visible')) {
        $other_vendor.slideUp('fast');
      }
    });
    
    /**
     * Tech specs on vendor pages
     */
    $('.page-node.node-type-vendor .node-spec-cat-title a')
    .append('<span class="plus-minus">+</span>')
    .bind('click', function() {
      var $this = $(this),
          $target = $($this.attr('href')),
          $plus_minus = $this.find('.plus-minus'),
          pm_text = $plus_minus.text();
      
      if (pm_text == '+') {
        $plus_minus.text('-');
      } else if (pm_text == '-') {
        $plus_minus.text('+');
      }
      
      $target.slideToggle('fast');
      return false;
    });
    
    $('a.scrollto').bind('click', function() {
      var $t = $(this),
          $targ = $($t.attr('href')),
          targ_html = $targ.html(),
          to_return = true;
      
      if (typeof targ_html == 'string') {
        $('html, body').animate({scrollTop: $targ.offset().top}, 250);
        to_return = false;
      }
      
      return to_return;
    });
    
    /**
     * More vendors
     */
    var more_vendors_tab = '<a id="js-more-vendors" href="#"><span class="more">More</span> <span class="vendors">Vendors</span></a>';
    more_vendors_tab += '<div class="tab-shadow top"></div><div class="tab-shadow bottom"></div>';
    
    $('#vendors-by-category-wrapper').prepend(more_vendors_tab);
    
    $('#js-more-vendors').bind('click', function() {
      $('#vendors-by-category .vendor-column.first-vendor').each(function() {
        var $old_first_cell = $(this),
            $new_first_cell = $old_first_cell.next(),
            $row = $old_first_cell.parent();
        
        $old_first_cell.animate({'opacity': 0}, 100, function() {
          $row.append($old_first_cell);
          $old_first_cell.removeAttr('style');
          $row.children().removeClass('first-vendor');
          $new_first_cell.addClass('first-vendor');
        });
      });
      
      return false;
    });
    
    $('#vendors-by-category a.collapse-expand-link')
    .append('<span class="plus-minus">+</span>')
    .bind('click', function() {
      var $this = $(this),
          $targ = $('#vendors-by-category-wrapper tr' + $this.attr('data-collapse-target')),
          plus_minus_txt = '+',
          targ_collapsed = $targ.hasClass('collapsed'),
          r_class = 'expanded',
          a_class = 'collapsed';
      
      if (targ_collapsed) {
        plus_minus_txt = '-';
        r_class = 'collapsed';
        a_class = 'expanded';
      }
      
      $targ.slideToggle('fast', function() {
        $this.find('.plus-minus').text(plus_minus_txt);
        $(this).removeClass(r_class).addClass(a_class);
      });
      
      return false;
    });
    
    $('#vendors-by-category-wrapper .bottom-line-button-row a.collapse-expand-link .plus-minus').text('-');
  });
})(jQuery);