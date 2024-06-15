<script setup lang="ts">
import loginVue from "@/components/login.vue"
import failVue from "@/components/fail.vue"
import userManageVue from "@/components/userManage.vue"
import { requestOverWs } from "@/store/requestOverWs";
import { ref } from "vue";
import md5 from 'js-md5';

const req = requestOverWs();

const openDoorCommand = ref({
  isSending: false,
  isSuccess: false,
  isFail: false
})
const authRes = ref({
  isSuccess: "",
  isFail: "",
  isProcessing: ""
})
const failDiag = ref({
  show: false,
  reason: "1111111111"
})
const fail = (reason: string) => {
  failDiag.value.show = true;
  failDiag.value.reason = reason;
}
const showLogin = ref(false);
let token: string = "";

req.events.on("open", async (data) => {
  authRes.value.isSuccess = "成功";
  authRes.value.isProcessing = "";
  try {
    let result;
    if (token)
      result = await req.query({
        type: "auth",
        auth: "token",
        token: token
      });
    else
      result = await req.query({
        type: "auth",
        auth: "user",
        username: data.username,
        password: md5.md5(data.password + "_FP_LOCK_SALT_UISGHPFOIGSUDI")
      });
    if (result.status) {
      authRes.value.isSuccess = "成功";
      authRes.value.isProcessing = "";
    }
  } catch (e: any) {
    fail(e.toString());
  }
})

async function login(data: { username: string, password: string }) {
  authRes.value.isProcessing = "正在连接到服务器";
  req.createWs();
}

async function openDoor() {
  openDoorCommand.value.isSending = true;
  openDoorCommand.value.isFail = false;
  openDoorCommand.value.isSuccess = false;
  try {
    await req.query({ type: "open" });
  } catch (e: any) {
    openDoorCommand.value.isFail = true;
    openDoorCommand.value.isSending = false;
  }
}

const urlParams = new URLSearchParams(window.location.search);
token = urlParams.get("token") ?? "";
if (token) {
  authRes.value.isProcessing = "正在验证token合法性";
  req.createWs();
} else {
  showLogin.value = true;
}
</script>

<template>
  <el-container style="text-align: center;">
    <el-header>
      <h2>
        访客系统
      </h2>
    </el-header>
    <el-main>
      <loginVue :show="showLogin" @login="login"></loginVue>
      <failVue :show="failDiag.show" :reason="failDiag.reason"></failVue>
      <el-alert v-if="authRes.isSuccess" :title="authRes.isSuccess" type="success" center show-icon />
      <el-alert v-if="authRes.isProcessing" :title="authRes.isProcessing" type="info" center show-icon />
      <el-alert v-if="authRes.isFail" :title="authRes.isFail" type="error" center show-icon />
      <el-button type="primary" :loading="false" @click="openDoor()">开门</el-button>
      <userManageVue :show="true"></userManageVue>
    </el-main>
  </el-container>
</template>

<style scoped></style>
