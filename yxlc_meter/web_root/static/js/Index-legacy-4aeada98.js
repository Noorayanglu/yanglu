!function(){function e(t){return e="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(e){return typeof e}:function(e){return e&&"function"==typeof Symbol&&e.constructor===Symbol&&e!==Symbol.prototype?"symbol":typeof e},e(t)}var t=["序列号","充电截止电芯电压","放电截止电芯电压","充电关口限制","放电关口限制","充电变压器限制","放电变压器限制","Cloud用户名","Cloud密码","Cloud Url","Cloud Client ID","数据保留天数","上传高速","上传中速","上传低速"];function r(e,t){if(null==e)return{};var r,n,a=function(e,t){if(null==e)return{};var r,n,a={},o=Object.keys(e);for(n=0;n<o.length;n++)r=o[n],t.indexOf(r)>=0||(a[r]=e[r]);return a}(e,t);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(n=0;n<o.length;n++)r=o[n],t.indexOf(r)>=0||Object.prototype.propertyIsEnumerable.call(e,r)&&(a[r]=e[r])}return a}function n(){"use strict";/*! regenerator-runtime -- Copyright (c) 2014-present, Facebook, Inc. -- license (MIT): https://github.com/facebook/regenerator/blob/main/LICENSE */n=function(){return r};var t,r={},a=Object.prototype,o=a.hasOwnProperty,l=Object.defineProperty||function(e,t,r){e[t]=r.value},u="function"==typeof Symbol?Symbol:{},i=u.iterator||"@@iterator",c=u.asyncIterator||"@@asyncIterator",f=u.toStringTag||"@@toStringTag";function d(e,t,r){return Object.defineProperty(e,t,{value:r,enumerable:!0,configurable:!0,writable:!0}),e[t]}try{d({},"")}catch(t){d=function(e,t,r){return e[t]=r}}function s(e,t,r,n){var a=t&&t.prototype instanceof w?t:w,o=Object.create(a.prototype),u=new I(n||[]);return l(o,"_invoke",{value:O(e,r,u)}),o}function p(e,t,r){try{return{type:"normal",arg:e.call(t,r)}}catch(e){return{type:"throw",arg:e}}}r.wrap=s;var m="suspendedStart",h="suspendedYield",v="executing",g="completed",y={};function w(){}function b(){}function x(){}var _={};d(_,i,(function(){return this}));var E=Object.getPrototypeOf,j=E&&E(E(N([])));j&&j!==a&&o.call(j,i)&&(_=j);var C=x.prototype=w.prototype=Object.create(_);function k(e){["next","throw","return"].forEach((function(t){d(e,t,(function(e){return this._invoke(t,e)}))}))}function L(t,r){function n(a,l,u,i){var c=p(t[a],t,l);if("throw"!==c.type){var f=c.arg,d=f.value;return d&&"object"==e(d)&&o.call(d,"__await")?r.resolve(d.__await).then((function(e){n("next",e,u,i)}),(function(e){n("throw",e,u,i)})):r.resolve(d).then((function(e){f.value=e,u(f)}),(function(e){return n("throw",e,u,i)}))}i(c.arg)}var a;l(this,"_invoke",{value:function(e,t){function o(){return new r((function(r,a){n(e,t,r,a)}))}return a=a?a.then(o,o):o()}})}function O(e,r,n){var a=m;return function(o,l){if(a===v)throw new Error("Generator is already running");if(a===g){if("throw"===o)throw l;return{value:t,done:!0}}for(n.method=o,n.arg=l;;){var u=n.delegate;if(u){var i=S(u,n);if(i){if(i===y)continue;return i}}if("next"===n.method)n.sent=n._sent=n.arg;else if("throw"===n.method){if(a===m)throw a=g,n.arg;n.dispatchException(n.arg)}else"return"===n.method&&n.abrupt("return",n.arg);a=v;var c=p(e,r,n);if("normal"===c.type){if(a=n.done?g:h,c.arg===y)continue;return{value:c.arg,done:n.done}}"throw"===c.type&&(a=g,n.method="throw",n.arg=c.arg)}}}function S(e,r){var n=r.method,a=e.iterator[n];if(a===t)return r.delegate=null,"throw"===n&&e.iterator.return&&(r.method="return",r.arg=t,S(e,r),"throw"===r.method)||"return"!==n&&(r.method="throw",r.arg=new TypeError("The iterator does not provide a '"+n+"' method")),y;var o=p(a,e.iterator,r.arg);if("throw"===o.type)return r.method="throw",r.arg=o.arg,r.delegate=null,y;var l=o.arg;return l?l.done?(r[e.resultName]=l.value,r.next=e.nextLoc,"return"!==r.method&&(r.method="next",r.arg=t),r.delegate=null,y):l:(r.method="throw",r.arg=new TypeError("iterator result is not an object"),r.delegate=null,y)}function V(e){var t={tryLoc:e[0]};1 in e&&(t.catchLoc=e[1]),2 in e&&(t.finallyLoc=e[2],t.afterLoc=e[3]),this.tryEntries.push(t)}function U(e){var t=e.completion||{};t.type="normal",delete t.arg,e.completion=t}function I(e){this.tryEntries=[{tryLoc:"root"}],e.forEach(V,this),this.reset(!0)}function N(r){if(r||""===r){var n=r[i];if(n)return n.call(r);if("function"==typeof r.next)return r;if(!isNaN(r.length)){var a=-1,l=function e(){for(;++a<r.length;)if(o.call(r,a))return e.value=r[a],e.done=!1,e;return e.value=t,e.done=!0,e};return l.next=l}}throw new TypeError(e(r)+" is not iterable")}return b.prototype=x,l(C,"constructor",{value:x,configurable:!0}),l(x,"constructor",{value:b,configurable:!0}),b.displayName=d(x,f,"GeneratorFunction"),r.isGeneratorFunction=function(e){var t="function"==typeof e&&e.constructor;return!!t&&(t===b||"GeneratorFunction"===(t.displayName||t.name))},r.mark=function(e){return Object.setPrototypeOf?Object.setPrototypeOf(e,x):(e.__proto__=x,d(e,f,"GeneratorFunction")),e.prototype=Object.create(C),e},r.awrap=function(e){return{__await:e}},k(L.prototype),d(L.prototype,c,(function(){return this})),r.AsyncIterator=L,r.async=function(e,t,n,a,o){void 0===o&&(o=Promise);var l=new L(s(e,t,n,a),o);return r.isGeneratorFunction(t)?l:l.next().then((function(e){return e.done?e.value:l.next()}))},k(C),d(C,f,"Generator"),d(C,i,(function(){return this})),d(C,"toString",(function(){return"[object Generator]"})),r.keys=function(e){var t=Object(e),r=[];for(var n in t)r.push(n);return r.reverse(),function e(){for(;r.length;){var n=r.pop();if(n in t)return e.value=n,e.done=!1,e}return e.done=!0,e}},r.values=N,I.prototype={constructor:I,reset:function(e){if(this.prev=0,this.next=0,this.sent=this._sent=t,this.done=!1,this.delegate=null,this.method="next",this.arg=t,this.tryEntries.forEach(U),!e)for(var r in this)"t"===r.charAt(0)&&o.call(this,r)&&!isNaN(+r.slice(1))&&(this[r]=t)},stop:function(){this.done=!0;var e=this.tryEntries[0].completion;if("throw"===e.type)throw e.arg;return this.rval},dispatchException:function(e){if(this.done)throw e;var r=this;function n(n,a){return u.type="throw",u.arg=e,r.next=n,a&&(r.method="next",r.arg=t),!!a}for(var a=this.tryEntries.length-1;a>=0;--a){var l=this.tryEntries[a],u=l.completion;if("root"===l.tryLoc)return n("end");if(l.tryLoc<=this.prev){var i=o.call(l,"catchLoc"),c=o.call(l,"finallyLoc");if(i&&c){if(this.prev<l.catchLoc)return n(l.catchLoc,!0);if(this.prev<l.finallyLoc)return n(l.finallyLoc)}else if(i){if(this.prev<l.catchLoc)return n(l.catchLoc,!0)}else{if(!c)throw new Error("try statement without catch or finally");if(this.prev<l.finallyLoc)return n(l.finallyLoc)}}}},abrupt:function(e,t){for(var r=this.tryEntries.length-1;r>=0;--r){var n=this.tryEntries[r];if(n.tryLoc<=this.prev&&o.call(n,"finallyLoc")&&this.prev<n.finallyLoc){var a=n;break}}a&&("break"===e||"continue"===e)&&a.tryLoc<=t&&t<=a.finallyLoc&&(a=null);var l=a?a.completion:{};return l.type=e,l.arg=t,a?(this.method="next",this.next=a.finallyLoc,y):this.complete(l)},complete:function(e,t){if("throw"===e.type)throw e.arg;return"break"===e.type||"continue"===e.type?this.next=e.arg:"return"===e.type?(this.rval=this.arg=e.arg,this.method="return",this.next="end"):"normal"===e.type&&t&&(this.next=t),y},finish:function(e){for(var t=this.tryEntries.length-1;t>=0;--t){var r=this.tryEntries[t];if(r.finallyLoc===e)return this.complete(r.completion,r.afterLoc),U(r),y}},catch:function(e){for(var t=this.tryEntries.length-1;t>=0;--t){var r=this.tryEntries[t];if(r.tryLoc===e){var n=r.completion;if("throw"===n.type){var a=n.arg;U(r)}return a}}throw new Error("illegal catch attempt")},delegateYield:function(e,r,n){return this.delegate={iterator:N(e),resultName:r,nextLoc:n},"next"===this.method&&(this.arg=t),y}},r}function a(e,t,r,n,a,o,l){try{var u=e[o](l),i=u.value}catch(c){return void r(c)}u.done?t(i):Promise.resolve(i).then(n,a)}function o(e){return function(){var t=this,r=arguments;return new Promise((function(n,o){var l=e.apply(t,r);function u(e){a(l,n,o,u,i,"next",e)}function i(e){a(l,n,o,u,i,"throw",e)}u(void 0)}))}}System.register(["./user-legacy-426b6ac6.js","./index-legacy-6cb99d33.js","./feedback-legacy-f8f30044.js","./@vue-legacy-55b7f08a.js","./index-legacy-2da0ae0d.js","./lodash-legacy-b10f79d7.js","./dayjs-legacy-d4cf0d68.js","./axios-legacy-45a42015.js","./element-plus-legacy-f3c78f77.js","./lodash-es-legacy-8e03b1de.js","./@vueuse-legacy-de216c25.js","./@element-plus-legacy-7308db8d.js","./@popperjs-legacy-876caf52.js","./@ctrl-legacy-33dbca08.js","./async-validator-legacy-aa1fd2de.js","./memoize-one-legacy-599ef04d.js","./escape-html-legacy-9d0f3640.js","./normalize-wheel-es-legacy-13d39621.js","./@floating-ui-legacy-82e00343.js","./vue-router-legacy-d2fecccb.js","./pinia-legacy-62dfefa8.js","./animate.css-legacy-280ccc6e.js"],(function(e,a){"use strict";var l,u,i,c,f,d,s,p,m,h,v,g,y,w,b,x,_,E;return{setters:[function(e){l=e.g,u=e.p},function(e){i=e.j},function(e){c=e.f},function(e){f=e.d,d=e.e,s=e.Z,p=e.i,m=e.ag,h=e.c,v=e.a,g=e.U,y=e.O,w=e.o,b=e.S,x=e.az,_=e.aA},function(e){E=e._},null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null],execute:function(){var a=document.createElement("style");a.textContent=".setup .newpower-box[data-v-6e20d15a]{display:flex}.setup .newpower-box .newpower-btn[data-v-6e20d15a]{margin-left:10px;color:#fff;background-color:#ce181e}.setup .newpower-text[data-v-6e20d15a]{width:100%;display:block}.setup .power-box[data-v-6e20d15a]{width:100%;height:40vh;margin-top:10px}.setup .power-box .power-box-box[data-v-6e20d15a]{width:100%;height:40vh}.setup .col-box[data-v-6e20d15a]{padding-right:10px}.setup .power-card[data-v-6e20d15a]{padding-top:10px}.setup .card-header span[data-v-6e20d15a]{font-weight:700}.setup .setvalue[data-v-6e20d15a]{width:45%}.setup .setvalue-value[data-v-6e20d15a]{margin-bottom:18px;margin-top:10px}.setup .setvalue-value-data[data-v-6e20d15a]{width:37%}.setup .setvalue-title[data-v-6e20d15a]{display:flex;margin-bottom:10px;text-align:center}.setup .power-name[data-v-6e20d15a]{text-align:center}.setup .timepick[data-v-6e20d15a]{width:57%;display:flex;align-items:center}.setup .timepick div[data-v-6e20d15a]{font-size:12px}.setup .col-box-time[data-v-6e20d15a]{display:flex;padding-right:10px;font-size:14px;line-height:32px}.setup .time-text-margin[data-v-6e20d15a]{padding-right:10px}.setup .add-btn[data-v-6e20d15a]{height:22px;line-height:22px;text-align:center;border:1px dashed #d9d9d9;border-radius:4px;font-size:12px;cursor:pointer}.setup .add-btn[data-v-6e20d15a]:hover{color:#409eff;border-color:#409eff}.setup .add-btn[data-v-6e20d15a]:active{color:#5c6fc5;border-color:#5c6fc5}.setup .del-btn[data-v-6e20d15a]{height:22px;line-height:22px;color:#f56c6c;text-align:center}.setup .add-btn[data-v-6e20d15a]{height:22px;line-height:22px;text-align:center;border:1px dashed #d9d9d9;border-radius:4px;cursor:pointer}.setup .tab-box[data-v-6e20d15a]{box-shadow:0 0 12px rgba(0,0,0,.12);margin-top:2px}[data-v-6e20d15a] .tabline div.el-card__header{height:32px;line-height:32px;padding-top:0;padding-bottom:0}[data-v-6e20d15a] .tabline div.el-card__body{padding:0}[data-v-6e20d15a] .tabline div.el-form-item{margin-bottom:0;padding-top:4px;padding-bottom:3px;border-bottom:1px solid #ebeef5}.el-form[data-v-6e20d15a]:nth-child(2n){background-color:#fafafa}\n",document.head.appendChild(a);var j={class:"setup"},C={class:"tab-box"},k=function(e){return x("data-v-6e20d15a"),e=e(),_(),e}((function(){return v("div",{class:"card-header"},[v("span",null,"控制")],-1)})),L={class:"newpower-box"},O={class:"newpower-box"},S={class:"newpower-box"},V={class:"newpower-box"},U={class:"newpower-box"},I={class:"newpower-box"},N={class:"newpower-box"},P={class:"newpower-box"},R={class:"newpower-box"},G=f({__name:"Index",setup:function(e){var a=d([]),f=d({}),x=d([]),_=d([]),E=d([]),G=d(15);d(""),s([{chargeP:"",startTime:"",endTime:""}]),d(),s({param:""}),d(),s({param:null}),d(),s({param:null}),d(),s({param:null}),d(),s({param:null}),d(),s({param:null}),d(),s({param:null});var T=d(),z=s({param:null}),D=d(),F=s({param:null}),A=d(),Y=s({param:null}),Z=d(),q=s({param:null}),B=d(),H=s({param:null});d(),s({param:null});var J=d(),K=s({param:null}),M=d(),Q=s({param:null}),W=d(),X=s({param:null}),$=d(),ee=s({param:null});p(o(n().mark((function e(){return n().wrap((function(e){for(;;)switch(e.prev=e.next){case 0:return e.next=2,ne(G.value);case 2:return _.value=e.sent,e.next=5,oe();case 5:case"end":return e.stop()}}),e)}))));var te=function(){var e=o(n().mark((function e(t,r){var a,o,l;return n().wrap((function(e){for(;;)switch(e.prev=e.next){case 0:return e.next=2,re(t,r);case 2:a=e.sent,o=a.map((function(e){return e.name})),l=a.map((function(e){return e.value})),E.value=o.map((function(e,t){return[e,l[t]]}));case 6:case"end":return e.stop()}}),e)})));return function(t,r){return e.apply(this,arguments)}}(),re=function(e,t){var r=e.map((function(e,r){return{name:e,value:t[r]}})),n=[];return n.push(r[0]),r.reduce((function(e,t){var r=null;return e.value!==t.value&&(r={name:t.name,value:e.value}),r?n.push(r,t):n.push(t),t})),n},ne=function(e){for(var t=60*e,r=86400/t,n=0,a=[],o=0;o<r;o++){var l=parseInt(n/3600),u=parseInt(n%3600/60);a.push(ae(l)+":"+ae(u)),n+=t}return a},ae=function(e){return e<10?"0"+e:e},oe=function(){l().then((function(e){var n=e;f.value={"Cloud 用户名":n["Cloud用户名"],"Cloud 密码":n["Cloud密码"],"Cloud Url":n["Cloud Url"],"Cloud Client ID":n["Cloud Client ID"],"数据保留天数":n["数据保留天数"],"上传高速":n["上传高速"],"上传中速":n["上传中速"],"上传低速":n["上传低速"]},a.value=i(f.value);n["序列号"],n["充电截止电芯电压"],n["放电截止电芯电压"],n["充电关口限制"],n["放电关口限制"],n["充电变压器限制"],n["放电变压器限制"],n["Cloud用户名"],n["Cloud密码"],n["Cloud Url"],n["Cloud Client ID"],n["数据保留天数"],n["上传高速"],n["上传中速"],n["上传低速"];var o=r(n,t);x.value=Object.values(o),te(_.value,x.value)})).catch((function(e){}))};return function(e,t){var r=m("el-table-column"),n=m("el-table"),o=m("el-col"),l=m("el-input"),i=m("el-button"),f=m("el-form-item"),d=m("el-form"),s=m("el-card"),p=m("el-row");return w(),h("div",j,[v("div",null,[g(p,null,{default:y((function(){return[g(o,{xs:24,sm:12,class:"col-box"},{default:y((function(){return[v("div",C,[g(n,{data:a.value,stripe:"",border:"",align:"center",style:{width:"100%"}},{default:y((function(){return[g(r,{type:"index",width:"60",align:"center",label:"序号"}),g(r,{prop:"name",label:"数据名称",align:"center"}),g(r,{prop:"value",label:"数值",align:"center"}),g(r,{prop:"unit",label:"单位",align:"center",width:"60"})]})),_:1},8,["data"])])]})),_:1}),g(o,{xs:24,sm:12},{default:y((function(){return[g(s,{class:"box-card tabline"},{header:y((function(){return[k]})),default:y((function(){return[v("div",null,[g(d,{ref_key:"formRef6",ref:T,model:z,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",L,[g(l,{modelValue:z.param,"onUpdate:modelValue":t[0]||(t[0]=function(e){return z.param=e}),class:"newpower-input",placeholder:"Cloud 用户名"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[1]||(t[1]=function(e){return function(e){if(e)if(null!==z.param){var t={cmd:101,param:z.param};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入Cloud 用户名")}(T.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef7",ref:D,model:F,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",O,[g(l,{modelValue:F.param,"onUpdate:modelValue":t[2]||(t[2]=function(e){return F.param=e}),class:"newpower-input",placeholder:"Cloud 密码"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[3]||(t[3]=function(e){return function(e){if(e)if(null!==F.param){var t={cmd:102,param:F.param};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入Cloud 密码")}(D.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef8",ref:A,model:Y,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",S,[g(l,{modelValue:Y.param,"onUpdate:modelValue":t[4]||(t[4]=function(e){return Y.param=e}),class:"newpower-input",placeholder:"Cloud Url"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[5]||(t[5]=function(e){return function(e){if(e)if(null!==Y.param){var t={cmd:103,param:Y.param};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入Cloud Url")}(A.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef10",ref:B,model:H,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",V,[g(l,{modelValue:H.param,"onUpdate:modelValue":t[6]||(t[6]=function(e){return H.param=e}),class:"newpower-input",placeholder:"Cloud Client ID"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[7]||(t[7]=function(e){return function(e){if(e)if(null!==H.param){var t={cmd:105,param:H.param};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入Cloud Client ID")}(B.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef12",ref:J,model:K,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",U,[g(l,{modelValue:K.param,"onUpdate:modelValue":t[8]||(t[8]=function(e){return K.param=e}),type:"number",class:"newpower-input",placeholder:"数据保留天数"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[9]||(t[9]=function(e){return function(e){if(e)if(null!==K.param){var t={cmd:200,param:Number(K.param)};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入数据保留天数")}(J.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef13",ref:M,model:Q,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",I,[g(l,{modelValue:Q.param,"onUpdate:modelValue":t[10]||(t[10]=function(e){return Q.param=e}),type:"number",class:"newpower-input",placeholder:"上传高速"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[11]||(t[11]=function(e){return function(e){if(e)if(null!==Q.param){var t={cmd:201,param:Number(Q.param)};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入上传高速")}(M.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef14",ref:W,model:X,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",N,[g(l,{modelValue:X.param,"onUpdate:modelValue":t[12]||(t[12]=function(e){return X.param=e}),type:"number",class:"newpower-input",placeholder:"上传中速"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[13]||(t[13]=function(e){return function(e){if(e)if(null!==X.param){var t={cmd:202,param:Number(X.param)};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入上传中速")}(W.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef15",ref:$,model:ee,"label-width":"50"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",P,[g(l,{modelValue:ee.param,"onUpdate:modelValue":t[14]||(t[14]=function(e){return ee.param=e}),type:"number",class:"newpower-input",placeholder:"上传低速"},null,8,["modelValue"]),g(i,{class:"newpower-btn",onClick:t[15]||(t[15]=function(e){return function(e){if(e)if(null!==ee.param){var t={cmd:203,param:Number(ee.param)};u(t).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))}else c.msgError("请输入上传低速")}($.value)})},{default:y((function(){return[b("设置")]})),_:1})])]})),_:1})]})),_:1},8,["model"]),g(d,{ref_key:"formRef9",ref:Z,model:q,"label-width":"130"},{default:y((function(){return[g(f,{label:"",prop:"param"},{default:y((function(){return[v("div",R,[g(i,{class:"newpower-btn",onClick:t[16]||(t[16]=function(e){Z.value&&u({cmd:104,param:0}).then((function(e){e?c.msgSuccess("操作成功"):c.msgError("操作失败"),oe()})).catch((function(e){c.msgError("操作失败")}))})},{default:y((function(){return[b("Cloud 注册")]})),_:1})])]})),_:1})]})),_:1},8,["model"])])]})),_:1})]})),_:1})]})),_:1})])])}}});e("default",E(G,[["__scopeId","data-v-6e20d15a"]]))}}}))}();
