import{g as e,p as a}from"./user-cc8e200f.js";import{j as l}from"./index-2a37d408.js";import{f as r}from"./feedback-5363c0cc.js";import{d as o,e as m,Z as u,i as t,ag as s,c as p,a as n,U as d,O as c,o as i,S as f,az as w,aA as b}from"./@vue-c5a747fc.js";import{_ as v}from"./index-e5357978.js";import"./lodash-20cd73ca.js";import"./dayjs-9ee440cb.js";import"./axios-5b55c11d.js";import"./element-plus-230cec57.js";import"./lodash-es-fb3d0246.js";import"./@vueuse-90035fc5.js";import"./@element-plus-388117bf.js";import"./@popperjs-b78c3215.js";import"./@ctrl-91de2ec7.js";import"./async-validator-cf877c1f.js";import"./memoize-one-63ab667a.js";import"./escape-html-92a447fa.js";import"./normalize-wheel-es-3222b0a2.js";import"./@floating-ui-9ca8b935.js";import"./vue-router-641e505b.js";import"./pinia-801b3705.js";/* empty css                    */const h={class:"setup"},_={class:"tab-box"},g=(e=>(w("data-v-6e20d15a"),e=e(),b(),e))((()=>n("div",{class:"card-header"},[n("span",null,"控制")],-1))),C={class:"newpower-box"},j={class:"newpower-box"},E={class:"newpower-box"},V={class:"newpower-box"},x={class:"newpower-box"},k={class:"newpower-box"},y={class:"newpower-box"},U={class:"newpower-box"},S={class:"newpower-box"},I=v(o({__name:"Index",setup(o){const w=m([]),b=m({}),v=m([]),I=m([]),R=m([]),D=m(15);m(""),u([{chargeP:"",startTime:"",endTime:""}]),m(),u({param:""}),m(),u({param:null}),m(),u({param:null}),m(),u({param:null}),m(),u({param:null}),m(),u({param:null}),m(),u({param:null});const N=m(),z=u({param:null}),O=m(),T=u({param:null}),A=m(),P=u({param:null}),Z=m(),q=u({param:null}),B=m(),F=u({param:null});m(),u({param:null});const G=m(),H=u({param:null}),J=m(),K=u({param:null}),L=m(),M=u({param:null}),Q=m(),W=u({param:null});t((async()=>{I.value=await Y(D.value),await ee()}));const X=(e,a)=>{const l=e.map(((e,l)=>({name:e,value:a[l]})));let r=[];return r.push(l[0]),l.reduce(((e,a)=>{let l=null;return e.value!==a.value&&(l={name:a.name,value:e.value}),l?r.push(l,a):r.push(a),a})),r},Y=e=>{let a=60*e,l=86400/a,r=0,o=[];for(let m=0;m<l;m++){let e=parseInt(r/3600),l=parseInt(r%3600/60);o.push($(e)+":"+$(l)),r+=a}return o},$=e=>e<10?"0"+e:e,ee=()=>{e().then((e=>{let a=e;b.value={"Cloud 用户名":a["Cloud用户名"],"Cloud 密码":a["Cloud密码"],"Cloud Url":a["Cloud Url"],"Cloud Client ID":a["Cloud Client ID"],"数据保留天数":a["数据保留天数"],"上传高速":a["上传高速"],"上传中速":a["上传中速"],"上传低速":a["上传低速"]},w.value=l(b.value);const{"序列号":r,"充电截止电芯电压":o,"放电截止电芯电压":m,"充电关口限制":u,"放电关口限制":t,"充电变压器限制":s,"放电变压器限制":p,"Cloud用户名":n,"Cloud密码":d,"Cloud Url":c,"Cloud Client ID":i,"数据保留天数":f,"上传高速":h,"上传中速":_,"上传低速":g,...C}=a;v.value=Object.values(C),(async(e,a)=>{let l=await X(e,a),r=l.map((e=>e.name)),o=l.map((e=>e.value));R.value=r.map(((e,a)=>[e,o[a]]))})(I.value,v.value)})).catch((e=>{}))};return(e,l)=>{const o=s("el-table-column"),m=s("el-table"),u=s("el-col"),t=s("el-input"),b=s("el-button"),v=s("el-form-item"),I=s("el-form"),R=s("el-card"),D=s("el-row");return i(),p("div",h,[n("div",null,[d(D,null,{default:c((()=>[d(u,{xs:24,sm:12,class:"col-box"},{default:c((()=>[n("div",_,[d(m,{data:w.value,stripe:"",border:"",align:"center",style:{width:"100%"}},{default:c((()=>[d(o,{type:"index",width:"60",align:"center",label:"序号"}),d(o,{prop:"name",label:"数据名称",align:"center"}),d(o,{prop:"value",label:"数值",align:"center"}),d(o,{prop:"unit",label:"单位",align:"center",width:"60"})])),_:1},8,["data"])])])),_:1}),d(u,{xs:24,sm:12},{default:c((()=>[d(R,{class:"box-card tabline"},{header:c((()=>[g])),default:c((()=>[n("div",null,[d(I,{ref_key:"formRef6",ref:N,model:z,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",C,[d(t,{modelValue:z.param,"onUpdate:modelValue":l[0]||(l[0]=e=>z.param=e),class:"newpower-input",placeholder:"Cloud 用户名"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[1]||(l[1]=e=>(e=>{if(!e)return;if(null===z.param)return void r.msgError("请输入Cloud 用户名");let l={cmd:101,param:z.param};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(N.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef7",ref:O,model:T,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",j,[d(t,{modelValue:T.param,"onUpdate:modelValue":l[2]||(l[2]=e=>T.param=e),class:"newpower-input",placeholder:"Cloud 密码"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[3]||(l[3]=e=>(e=>{if(!e)return;if(null===T.param)return void r.msgError("请输入Cloud 密码");let l={cmd:102,param:T.param};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(O.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef8",ref:A,model:P,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",E,[d(t,{modelValue:P.param,"onUpdate:modelValue":l[4]||(l[4]=e=>P.param=e),class:"newpower-input",placeholder:"Cloud Url"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[5]||(l[5]=e=>(e=>{if(!e)return;if(null===P.param)return void r.msgError("请输入Cloud Url");let l={cmd:103,param:P.param};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(A.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef10",ref:B,model:F,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",V,[d(t,{modelValue:F.param,"onUpdate:modelValue":l[6]||(l[6]=e=>F.param=e),class:"newpower-input",placeholder:"Cloud Client ID"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[7]||(l[7]=e=>(e=>{if(!e)return;if(null===F.param)return void r.msgError("请输入Cloud Client ID");let l={cmd:105,param:F.param};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(B.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef12",ref:G,model:H,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",x,[d(t,{modelValue:H.param,"onUpdate:modelValue":l[8]||(l[8]=e=>H.param=e),type:"number",class:"newpower-input",placeholder:"数据保留天数"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[9]||(l[9]=e=>(e=>{if(!e)return;if(null===H.param)return void r.msgError("请输入数据保留天数");let l={cmd:200,param:Number(H.param)};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(G.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef13",ref:J,model:K,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",k,[d(t,{modelValue:K.param,"onUpdate:modelValue":l[10]||(l[10]=e=>K.param=e),type:"number",class:"newpower-input",placeholder:"上传高速"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[11]||(l[11]=e=>(e=>{if(!e)return;if(null===K.param)return void r.msgError("请输入上传高速");let l={cmd:201,param:Number(K.param)};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(J.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef14",ref:L,model:M,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",y,[d(t,{modelValue:M.param,"onUpdate:modelValue":l[12]||(l[12]=e=>M.param=e),type:"number",class:"newpower-input",placeholder:"上传中速"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[13]||(l[13]=e=>(e=>{if(!e)return;if(null===M.param)return void r.msgError("请输入上传中速");let l={cmd:202,param:Number(M.param)};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(L.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef15",ref:Q,model:W,"label-width":"50"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",U,[d(t,{modelValue:W.param,"onUpdate:modelValue":l[14]||(l[14]=e=>W.param=e),type:"number",class:"newpower-input",placeholder:"上传低速"},null,8,["modelValue"]),d(b,{class:"newpower-btn",onClick:l[15]||(l[15]=e=>(e=>{if(!e)return;if(null===W.param)return void r.msgError("请输入上传低速");let l={cmd:203,param:Number(W.param)};a(l).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(Q.value))},{default:c((()=>[f("设置")])),_:1})])])),_:1})])),_:1},8,["model"]),d(I,{ref_key:"formRef9",ref:Z,model:q,"label-width":"130"},{default:c((()=>[d(v,{label:"",prop:"param"},{default:c((()=>[n("div",S,[d(b,{class:"newpower-btn",onClick:l[16]||(l[16]=e=>(e=>{if(!e)return;a({cmd:104,param:0}).then((e=>{e?r.msgSuccess("操作成功"):r.msgError("操作失败"),ee()})).catch((e=>{r.msgError("操作失败")}))})(Z.value))},{default:c((()=>[f("Cloud 注册")])),_:1})])])),_:1})])),_:1},8,["model"])])])),_:1})])),_:1})])),_:1})])])}}}),[["__scopeId","data-v-6e20d15a"]]);export{I as default};
