(function ($) {
  Drupal.behaviors.menuMultiLevel = {
    attach: function(settings, context) {
      var buildMenuMultiLevel = function(menuSel, fadeSettings) {
        var $mainMenu = $(menuSel),
            $headers = $mainMenu.find('ul').parent();
        
        $headers.each(function(i) {
          var $curobj = $(this),
              $subul = $curobj.find('ul:eq(0)'),
              dimensions = {
                w: $curobj.outerWidth(),
                h: $curobj.outerHeight(),
                subulw: $subul.outerWidth(),
                subulh: $subul.outerHeight()
              },
              $ulParents = $curobj.parents('ul'),
              parentsCount = $ulParents.length,
              isTopHeader = parentsCount == 1 ? true : false;
          
          if (!isTopHeader) {
            $subul.css({top: 0});
          }
          
          $curobj.hover(
            function(e) {
              var $targetul = $curobj.children('ul:eq(0)'),
                  offsets = {
                    left: $curobj.offset().left,
                    top: $curobj.offset().top
                  };
              
              $targetul.css({display: 'block', visibility: 'hidden'});
              dimensions.subulw = $targetul.outerWidth();
              $targetul.css({display: 'none', visibility: 'visible'});
              dimensions.w = $($ulParents[0]).outerWidth();
              
              if (!isTopHeader) {
                var menuleft = dimensions.w;
                
                if (offsets.left + menuleft + dimensions.subulw > $(window).width()) {
                  menuleft = -dimensions.subulw;
                }
                
                $targetul.css({left: menuleft + 'px'});
              }
              
              $targetul.fadeIn(fadeSettings.overduration);
            },
            function(e) {
              $(this).children('ul:eq(0)').fadeOut(fadeSettings.outduration);
            }
          );
        });
        
        $mainMenu.find("ul").css({display:'none', visibility:'visible'})
      }
      
      var overrideDefault = function() {
        if (typeof Drupal.settings.menuMultiLevel == 'undefined') return false;
        if (typeof Drupal.settings.menuMultiLevel.menus == 'undefined') return false;
        if (!Drupal.settings.menuMultiLevel.menus.length) return false;
        
        return true;
      }
      
      var fs = {
        overduration: 350,
        outduration: 100
      }
      
      if (overrideDefault()) {
        $.each(Drupal.settings.menuMultiLevel.menus, function(index, value) {
          buildMenuMultiLevel(value, fs);
        });
      }
      else {
        // Add default menu stuff here
      }
    }
  }
})(jQuery);
