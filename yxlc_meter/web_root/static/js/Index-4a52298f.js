import{u as e,b as l}from"./vue-router-641e505b.js";import{u as a,_ as s}from"./index-e5357978.js";import{d as t,e as o,j as n,f as i,ag as r,c as d,U as u,O as c,o as p,a as m,u as f,R as v,F as g,a7 as x,M as y,S as _,T as b,J as h,az as j,aA as k}from"./@vue-c5a747fc.js";import"./pinia-801b3705.js";import"./element-plus-230cec57.js";import"./lodash-es-fb3d0246.js";import"./@vueuse-90035fc5.js";import"./@element-plus-388117bf.js";import"./@popperjs-b78c3215.js";import"./@ctrl-91de2ec7.js";import"./dayjs-9ee440cb.js";import"./async-validator-cf877c1f.js";import"./memoize-one-63ab667a.js";import"./escape-html-92a447fa.js";import"./normalize-wheel-es-3222b0a2.js";import"./@floating-ui-9ca8b935.js";/* empty css                    */const w=e=>(j("data-v-cbfeafdb"),e=e(),k(),e),z={class:"admin"},F={key:0,class:"logo"},C={class:"logo-box"},I={key:0,class:"logoname"},L=[w((()=>m("div",{class:"logoname-one"},"裕兴股份智慧终端",-1))),w((()=>m("div",{class:"logoname-two"},"电表采集",-1)))],M={key:1,class:"logo"},T={key:0,style:{"padding-left":"16px"}},$={style:{"padding-left":"16px"}},A={key:0,style:{"padding-left":"16px"}},E={class:"display-flex ai-center jc-space-between flex-1 pl-20"},J={class:"display-flex ai-center ac-center"},O=w((()=>m("div",{class:"propname"},"裕兴股份智慧工商业储能电站",-1))),P=["onClick"],R={class:"display-flex ai-center user"},S={class:"nav-menu-item"},U={class:"display-flex ai-center"},q={class:"el-dropdown-link cursor-pointer ml-6 fs-14"},B={style:{background:"#fff",padding:"20px"}},D=s(t({__name:"Index",setup(s){const t=a(),j=e();l();const k=o(!1),w=o(!0),D=n((()=>t.menu||[])),G=n((()=>t.breadcrumb||[]));i(k,(e=>{w.value=!w.value,setTimeout((()=>{w.value=!w.value}),100)}));const H=()=>{t.logout()};return(e,l)=>{const a=r("el-image"),s=r("svg-icon"),t=r("el-menu-item"),o=r("el-menu-item-group"),n=r("el-sub-menu"),i=r("el-menu"),K=r("el-aside"),N=r("Expand"),Q=r("Fold"),V=r("el-icon"),W=r("el-breadcrumb-item"),X=r("el-breadcrumb"),Y=r("Monitor"),Z=r("router-link"),ee=r("arrow-down"),le=r("el-dropdown-item"),ae=r("el-dropdown-menu"),se=r("el-dropdown"),te=r("el-header"),oe=r("router-view"),ne=r("el-scrollbar"),ie=r("el-main"),re=r("el-container");return p(),d("div",z,[u(re,{class:"height-percent-100"},{default:c((()=>[u(K,{width:k.value?"65px":"220px",class:"aside scrollbar"},{default:c((()=>[w.value?(p(),d("div",F,[m("div",C,[u(a,{src:f("/static/png/logonew-af380e46.png")},null,8,["src"])]),k.value?v("",!0):(p(),d("span",I,L))])):(p(),d("div",M)),u(i,{"default-active":e.$route.path,collapse:k.value,class:"menu","text-color":"rgba(255, 255, 255, .9)",router:"","collapse-transition":!1,"active-text-color":"#1990FF","background-color":"rgba(38, 52, 69, .9)"},{default:c((()=>[(p(!0),d(g,null,x(D.value,((e,l)=>(p(),d(g,null,[e.childList.length?(p(),y(n,{key:l,index:e.url},{title:c((()=>[u(s,{name:e.icon},null,8,["name"]),k.value?v("",!0):(p(),d("span",T,b(e.name),1))])),default:c((()=>[u(o,null,{default:c((()=>[(p(!0),d(g,null,x(e.childList,((e,l)=>(p(),y(t,{key:l,index:e.url},{title:c((()=>[u(s,{name:e.icon},null,8,["name"]),m("span",$,b(e.name),1)])),_:2},1032,["index"])))),128))])),_:2},1024)])),_:2},1032,["index"])):(p(),y(t,{key:l+1,index:e.url},{default:c((()=>[u(s,{name:e.icon},null,8,["name"]),k.value?v("",!0):(p(),d("div",A,b(e.name),1))])),_:2},1032,["index"]))],64)))),256))])),_:1},8,["default-active","collapse"])])),_:1},8,["width"]),u(re,null,{default:c((()=>[u(te,{class:"display-flex fd-column jc-center header"},{default:c((()=>[m("div",E,[m("div",J,[u(V,{class:"mr-12",size:24,onClick:l[0]||(l[0]=e=>k.value=!k.value)},{default:c((()=>[k.value?(p(),y(N,{key:0})):(p(),y(Q,{key:1}))])),_:1}),O,u(X,{"separator-class":"el-icon-arrow-right"},{default:c((()=>[(p(!0),d(g,null,x(G.value,((e,l)=>(p(),y(W,{key:l},{default:c((()=>[m("span",{class:"cursor-pointer",style:h({color:G.value.length-1===l?"#1990FF":"#999999"}),onClick:e=>(e=>{e+1!==G.value.length&&0!==e&&j.go(e+1-G.value.length)})(l)},b(e),13,P)])),_:2},1024)))),128))])),_:1})]),m("div",R,[u(Z,{class:"h100",target:"_blank",title:"首页",to:"/"},{default:c((()=>[m("div",S,[u(V,{color:"#000",style:{"font-size":"20px"}},{default:c((()=>[u(Y)])),_:1})])])),_:1}),u(se,{size:"large",trigger:"click"},{dropdown:c((()=>[u(ae,null,{default:c((()=>[u(le,{onClick:H},{default:c((()=>[_(" 退出登录 ")])),_:1})])),_:1})])),default:c((()=>[m("div",U,[m("span",q,[_(" 管理员"),u(V,{class:"el-icon--right"},{default:c((()=>[u(ee)])),_:1})])])])),_:1})])])])),_:1}),u(ie,{class:"scrollbar-y",style:{height:"calc(100vh - 140px)",background:"#f4f4f4",padding:"0"}},{default:c((()=>[u(ne,null,{default:c((()=>[m("div",B,[(p(),y(oe,{key:e.$route.fullPath}))])])),_:1})])),_:1})])),_:1})])),_:1})])}}}),[["__scopeId","data-v-cbfeafdb"]]);export{D as default};