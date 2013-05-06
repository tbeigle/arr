var arrfv_timeout_trigger;

(function($) {
  $(document).ready(function() {
    if ($('body').hasClass('front')) {
      var arrfv_animate = false,
          arrfv_never_animate = false,
          $arrfv_block = $('#block-arr-site-front-vendors'),
          $arrfv_slider = $arrfv_block.find('.front-vendors-slider'),
          $arrfv_slides = $arrfv_slider.find('li'),
          $arrfv_slide_f = $arrfv_slider.find('li:first-child'),
          $arrfv_slide_l = $arrfv_slider.find('li:last-child'),
          arrfv_visible_width = 0,
          arrfv_slide_f_width = 0,
          arrfv_slide_l_width = 0,
          arrfv_slider_width = 0,
          arrfv_slider_left_lock = 0,
          arrfv_slider_right_lock = 0,
          arrfv_timeout;
      
      $arrfv_block.append('<div class="overlay left img-wrapper"></div>');
      $arrfv_block.append('<div class="overlay right img-wrapper"></div>');
            
      /**
       * Timer trigger function
       */
      arrfv_timeout_trigger = function () {        
        clearTimeout(arrfv_timeout);
        arrfv_wait_to_load();
      }
      
      /**
       * Timer init function
       */
      function arrfv_timeout_init() {
        arrfv_timeout = setTimeout('arrfv_timeout_trigger()', 1000);
      }
      
      arrfv_timeout_init();
      
      /**
       * Delays until all the images are loaded
       */
      function arrfv_wait_to_load() {
        var slide_count = 0,
            slides_counted = 0;
        
        $arrfv_slides.each(function() {
          ++slide_count;
          
          var $t = $(this),
              outer_width = $t.outerWidth();
          
          if ($t.outerWidth() <= 0) {
            arrfv_timeout_init();
            return;
          } else {
            ++slides_counted;
          }
        });
        
        if (slides_counted < slide_count) {
          arrfv_timeout_init();
        } else {
          arrfv_set_vars();
        }
      }
      
      /**
       * Sets the variables
       */
      function arrfv_set_vars() {
        var $ol_r = $arrfv_block.find('.overlay.right'),
            arrfv_block_width = $arrfv_block.outerWidth(),
            arrfv_overlay_width = $ol_r.outerWidth();
        
        $arrfv_slider.addClass('fading-in').css({'left': arrfv_overlay_width+'px', 'visibility': 'visible', 'opacity': 0}).animate({'opacity': 1}, 'fast', function() {
          $(this).removeClass('fading-in');
        });
        
        arrfv_slide_f_width = $arrfv_slide_f.outerWidth();
        arrfv_slide_l_width = $arrfv_slide_l.outerWidth();
        arrfv_slider_width = $arrfv_slider.outerWidth();
        arrfv_slider_left_lock = -(arrfv_slider_width - arrfv_block_width + arrfv_slide_l_width - arrfv_overlay_width);
        arrfv_slider_right_lock = arrfv_overlay_width;
        arrfv_visible_width = arrfv_block_width - 2 * arrfv_overlay_width;
        arrfv_never_animate = (arrfv_visible_width >= arrfv_slider_width);
        
        if (!arrfv_never_animate) {
          $arrfv_block.find('.overlay').each(function() {
            var $o = $(this),
                affrv_ol_left = ($o.hasClass('left'));
            
            $o
            .css({'pointer-events': 'auto'})
            .bind('mouseenter', function() {
              if (!$arrfv_slider.hasClass('fading-in')) {
                if (affrv_ol_left) {
                  arrfv_animate = 'right';
                } else {
                  arrfv_animate = 'left';
                }
                
                arrfv_run_animation();
              }
            })
            .bind('mouseleave', function() {
              if (!$arrfv_slider.hasClass('fading-in')) {
                $arrfv_slider.stop();
                arrfv_animate = false;
              }
            });
          });
        }
      }
      
      /**
       * Perform horizontal scrolling animations
       */
      function arrfv_run_animation() {
        if (arrfv_animate !== false) {
          var cur_left = parseInt($arrfv_slider.css('left').replace(/px/g, ''));
          
          if (arrfv_animate == 'left' && cur_left > arrfv_slider_left_lock) {
            $arrfv_slider.animate({'left': arrfv_slider_left_lock+'px'}, 1500);
            /*$arrfv_slider.animate({'left': '-=1px'}, 10, function() {
              arrfv_run_animation();
            });*/
          } else if (arrfv_animate == 'right' && cur_left < arrfv_slider_right_lock) {
            $arrfv_slider.animate({'left': arrfv_slider_right_lock+'px'}, 1500);
            /*$arrfv_slider.animate({'left': '+=1px'}, 10, function() {
              arrfv_run_animation();
            });*/
          }
        }
      }
    }
  });
})(jQuery);