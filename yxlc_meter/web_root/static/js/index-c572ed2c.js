function e(){import.meta.url,import("_").catch((()=>1))}import{ag as t,o as n,M as o,U as r,ay as a,d as s,j as i,c,a as m,K as l,as as u}from"./@vue-c5a747fc.js";import{c as p,a as d}from"./vue-router-641e505b.js";import{d as h,c as f}from"./pinia-801b3705.js";import{i as g}from"./element-plus-230cec57.js";/* empty css                    */import{Q as _}from"./@element-plus-388117bf.js";import"./lodash-es-fb3d0246.js";import"./@vueuse-90035fc5.js";import"./@popperjs-b78c3215.js";import"./@ctrl-91de2ec7.js";import"./dayjs-9ee440cb.js";import"./async-validator-cf877c1f.js";import"./memoize-one-63ab667a.js";import"./escape-html-92a447fa.js";import"./normalize-wheel-es-3222b0a2.js";import"./@floating-ui-9ca8b935.js";!function(){const e=document.createElement("link").relList;if(!(e&&e.supports&&e.supports("modulepreload"))){for(const e of document.querySelectorAll('link[rel="modulepreload"]'))t(e);new MutationObserver((e=>{for(const n of e)if("childList"===n.type)for(const e of n.addedNodes)"LINK"===e.tagName&&"modulepreload"===e.rel&&t(e)})).observe(document,{childList:!0,subtree:!0})}function t(e){if(e.ep)return;e.ep=!0;const t=function(e){const t={};return e.integrity&&(t.integrity=e.integrity),e.referrerPolicy&&(t.referrerPolicy=e.referrerPolicy),"use-credentials"===e.crossOrigin?t.credentials="include":"anonymous"===e.crossOrigin?t.credentials="omit":t.credentials="same-origin",t}(e);fetch(e.href,t)}}();const y=(e,t)=>{const n=e.__vccOpts||e;for(const[o,r]of t)n[o]=r;return n};const E=y({},[["render",function(e,s){const i=t("router-view");return n(),o(a,null,[r(i)],1024)}]]),v={},I=function(e,t,n){if(!t||0===t.length)return e();const o=document.getElementsByTagName("link");return Promise.all(t.map((e=>{if((e=function(e){return"/"+e}(e))in v)return;v[e]=!0;const t=e.endsWith(".css"),r=t?'[rel="stylesheet"]':"";if(!!n)for(let n=o.length-1;n>=0;n--){const r=o[n];if(r.href===e&&(!t||"stylesheet"===r.rel))return}else if(document.querySelector('link[href="'.concat(e,'"]').concat(r)))return;const a=document.createElement("link");return a.rel=t?"stylesheet":"modulepreload",t||(a.as="script",a.crossOrigin=""),a.href=e,document.head.appendChild(a),t?new Promise(((t,n)=>{a.addEventListener("load",t),a.addEventListener("error",(()=>n(new Error("Unable to preload CSS for ".concat(e)))))})):void 0}))).then((()=>e())).catch((e=>{const t=new Event("vite:preloadError",{cancelable:!0});if(t.payload=e,window.dispatchEvent(t),!t.defaultPrevented)throw e}))},b={key:"like_admin_",set(e,t,n){e=this.getKey(e);let o={expire:n?this.time()+n:"",value:t};"object"==typeof o&&(o=JSON.stringify(o));try{window.localStorage.setItem(e,o)}catch(r){return null}},get(e){e=this.getKey(e);try{const t=window.localStorage.getItem(e);if(!t)return null;const{value:n,expire:o}=JSON.parse(t);return o&&o<this.time()?(window.localStorage.removeItem(e),null):n}catch(t){return null}},time:()=>Math.round((new Date).getTime()/1e3),remove(e){e=this.getKey(e),window.localStorage.removeItem(e)},getKey(e){return this.key+e}},j="token";function L(){return b.get(j)}function O(){P().resetState(),b.remove(j),function(){x.removeRoute(T);const{routes:e}=P();e.forEach((e=>{const t=e.name;t&&x.hasRoute(t)&&x.removeRoute(t)}))}()}var w=(e=>(e.LOGIN="/login",e.ERROR_403="/403",e.ERROR_404="/:pathMatch(.*)*",e.INDEX="/",e))(w||{});const R=[{icon:"shouye",name:"首页",url:"/admin/dashboard",paths:"admin/dashboard",childList:[]},{icon:"setup",name:"参数配置",url:"/admin/setup",paths:"admin/setup",childList:[]},{icon:"monitor",name:"计量系统",url:"/admin/metering",paths:"admin/metering",childList:[]},{icon:"online",name:"通信系统",url:"/admin/comm",paths:"admin/comm",childList:[]},{icon:"upgrade",name:"固件升级",url:"/admin/upgrade",paths:"admin/upgrade",childList:[]},{icon:"event",name:"事件",url:"/admin/log",paths:"admin/log",childList:[]}],P=h({id:"user",state:()=>({token:L()||"",userInfo:{},routes:[],breadcrumb:[],menu:[],perms:[]}),getters:{},actions:{resetState(){this.token="",this.userInfo={},this.breadcrumb=[],this.perms=[]},login(e){var t;this.token=e.token,t=e,b.set(j,t.token,86400)},logout:()=>new Promise(((e,t)=>{x.push(w.LOGIN),O()})),getUserInfo(){},getMenu(){return new Promise(((e,t)=>{const n=R;this.menu=n,this.routes=N(n),e(n)}))}}});var k=(e=>(e.CATALOGUE="M",e.MENU="C",e.BUTTON="A",e))(k||{});function A(e){return/^(https?:|mailto:|tel:)/.test(e)}function N(e,t=!0){return e.map((e=>{const n=function(e,t){const n={path:A(e.paths)?e.paths:t?"/".concat(e.paths):e.paths,name:Symbol(e.paths),meta:{hidden:!e.isShow,keepAlive:!!e.isCache,title:e.menuName,perms:e.perms,query:e.params,icon:e.menuIcon,type:e.menuType,activeMenu:e.selected}};switch(e.menuType){case k.CATALOGUE:n.component=t?LAYOUT:RouterView,e.children||(n.component=RouterView);break;case k.MENU:n.component=loadRouteView(e.component)}return n}(e,t);return null!=e.children&&e.children&&e.children.length&&(n.children=N(e.children,!1)),n}))}const S=[{path:"/",name:"App",redirect:"/login",component:E,children:[]},{path:"/admin",name:"Admin",meta:{title:"后台管理"},component:()=>I((()=>import("./Index-f4be24d9.js")),["static/js/Index-f4be24d9.js","static/js/vue-router-641e505b.js","static/js/@vue-c5a747fc.js","static/js/pinia-801b3705.js","static/js/element-plus-230cec57.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/dayjs-9ee440cb.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/css/Index-3dc49ff9.css","static/css/animate-cf768824.css"]),redirect:"/admin/dashboard",children:[{path:"dashboard",name:"首页",meta:{title:"首页",breadcrumb:["后台管理","首页"]},component:()=>I((()=>import("./Index-518cfdda.js")),["static/js/Index-518cfdda.js","static/js/index-641172c8.js","static/js/lodash-20cd73ca.js","static/js/dayjs-9ee440cb.js","static/js/axios-5b55c11d.js","static/js/feedback-5363c0cc.js","static/js/element-plus-230cec57.js","static/js/@vue-c5a747fc.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/css/Index-d4288eae.css","static/css/animate-cf768824.css"])},{path:"setup",name:"参数配置",meta:{title:"参数配置",breadcrumb:["后台管理","参数配置"]},component:()=>I((()=>import("./Index-9fd2eece.js")),["static/js/Index-9fd2eece.js","static/js/user-347ed06b.js","static/js/index-641172c8.js","static/js/lodash-20cd73ca.js","static/js/dayjs-9ee440cb.js","static/js/axios-5b55c11d.js","static/js/feedback-5363c0cc.js","static/js/element-plus-230cec57.js","static/js/@vue-c5a747fc.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/css/Index-2ab7ac74.css","static/css/animate-cf768824.css"])},{path:"metering",name:"计量系统",meta:{title:"计量系统",breadcrumb:["后台管理","计量系统"]},component:()=>I((()=>import("./Index-88451246.js")),["static/js/Index-88451246.js","static/js/user-347ed06b.js","static/js/index-641172c8.js","static/js/lodash-20cd73ca.js","static/js/dayjs-9ee440cb.js","static/js/axios-5b55c11d.js","static/js/feedback-5363c0cc.js","static/js/element-plus-230cec57.js","static/js/@vue-c5a747fc.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/css/Index-3e3a2a46.css","static/css/animate-cf768824.css"])},{path:"comm",name:"通信系统",meta:{title:"通信系统",breadcrumb:["后台管理","通信系统"]},component:()=>I((()=>import("./Index-9b5c60ba.js")),["static/js/Index-9b5c60ba.js","static/js/user-347ed06b.js","static/js/index-641172c8.js","static/js/lodash-20cd73ca.js","static/js/dayjs-9ee440cb.js","static/js/axios-5b55c11d.js","static/js/feedback-5363c0cc.js","static/js/element-plus-230cec57.js","static/js/@vue-c5a747fc.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/css/Index-fdcc2bfc.css","static/css/animate-cf768824.css"])},{path:"upgrade",name:"固件升级",meta:{title:"固件升级",breadcrumb:["后台管理","固件升级"]},component:()=>I((()=>import("./Index-194a2946.js")),["static/js/Index-194a2946.js","static/js/@vue-c5a747fc.js","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/js/element-plus-230cec57.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/dayjs-9ee440cb.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/css/animate-cf768824.css"])},{path:"log",name:"事件",meta:{title:"事件",breadcrumb:["后台管理","事件"]},component:()=>I((()=>import("./Index-74c92f8b.js")),["static/js/Index-74c92f8b.js","static/js/@vue-c5a747fc.js","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/js/element-plus-230cec57.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/dayjs-9ee440cb.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/css/animate-cf768824.css"])}]},{path:"/:pathMatch(.*)*",name:"NotFound",meta:{title:"错误页面"},component:()=>I((()=>import("./NotFound-10ab387f.js")),["static/js/NotFound-10ab387f.js","static/js/@vue-c5a747fc.js","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/js/element-plus-230cec57.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/dayjs-9ee440cb.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/css/animate-cf768824.css"])},{path:"/login",name:"Login",meta:{title:"登录"},component:()=>I((()=>import("./Index-a0a991af.js")),["static/js/Index-a0a991af.js","static/js/@vue-c5a747fc.js","static/js/feedback-5363c0cc.js","static/js/element-plus-230cec57.js","static/js/lodash-es-fb3d0246.js","static/js/@vueuse-90035fc5.js","static/js/@element-plus-388117bf.js","static/js/@popperjs-b78c3215.js","static/js/@ctrl-91de2ec7.js","static/js/dayjs-9ee440cb.js","static/js/async-validator-cf877c1f.js","static/js/memoize-one-63ab667a.js","static/js/escape-html-92a447fa.js","static/js/normalize-wheel-es-3222b0a2.js","static/js/@floating-ui-9ca8b935.js","static/css/element-plus-8a0c7673.css","static/js/vue-router-641e505b.js","static/js/pinia-801b3705.js","static/css/Index-485dba92.css","static/css/animate-cf768824.css"])}],T=Symbol();const x=p({history:d(),routes:S}),D=w.LOGIN;w.INDEX;const M=[w.LOGIN,w.ERROR_403];x.beforeEach((async(e,t,n)=>{document.title=e.meta.title+" - 裕兴股份电表采集";const o=P();if(await o.getMenu(),o.routes,M.includes(e.path))n();else if(o.token)try{await o.getMenu();o.routes;n()}catch(r){O(),n({path:D,query:{redirect:e.fullPath}})}else n({path:D,query:{redirect:e.fullPath}})})),x.afterEach((()=>{}));const V=["xlink:href"],U=y(s({__name:"svgIcon",props:{name:{type:String,required:!0},color:{type:String,default:""}},setup(e){const t=e,o=i((()=>"#icon-".concat(t.name))),r=i((()=>t.name?"svg-icon icon-".concat(t.name):"svg-icon"));return(t,a)=>(n(),c("svg",l({class:r.value},t.$attrs,{style:{color:e.color}}),[m("use",{"xlink:href":o.value},null,8,V)],16))}}),[["__scopeId","data-v-8de19606"]]),q=u(E),C=f();for(const[G,K]of Object.entries(_))q.component(G,K);q.use(g,{size:"small",zIndex:3e3}),q.use(x),q.use(C),q.component("SvgIcon",U),q.mount("#app");export{w as P,y as _,e as __vite_legacy_guard,O as c,L as g,x as r,P as u};
