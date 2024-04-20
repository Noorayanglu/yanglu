System.register(["./dayjs-legacy-d4cf0d68.js"],(function(e,t){"use strict";var r;return{setters:[function(e){r=e.g}],execute:function(){
/*!
       * escape-html
       * Copyright(c) 2012-2013 TJ Holowaychuk
       * Copyright(c) 2015 Andreas Lubbe
       * Copyright(c) 2015 Tiancheng "Timothy" Gu
       * MIT Licensed
       */
var t=/["'&<>]/;e("e",r((function(e){var r,a=""+e,s=t.exec(a);if(!s)return a;var n="",c=0,u=0;for(c=s.index;c<a.length;c++){switch(a.charCodeAt(c)){case 34:r="&quot;";break;case 38:r="&amp;";break;case 39:r="&#39;";break;case 60:r="&lt;";break;case 62:r="&gt;";break;default:continue}u!==c&&(n+=a.substring(u,c)),u=c+1,n+=r}return u!==c?n+a.substring(u,c):n})))}}}));
