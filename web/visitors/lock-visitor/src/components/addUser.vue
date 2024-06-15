<template>
  <el-dialog v-model="props.show" :ref="ruleFormRef" title="添加账号" width="500" align-center>
    <el-form label-position="top" label-width="auto" :model="form" style="max-width: 600px">
      <el-form-item label="用户名">
        <el-input v-model="form.username" />
      </el-form-item>
      <el-form-item label="密码">
        <el-input v-model="form.password" />
      </el-form-item>
      <el-form-item label="确认密码">
        <el-input v-model="form.confirmPassword" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="loading" @click="submit(form)">
          提交
        </el-button>
      </el-form-item>
    </el-form>
  </el-dialog>
</template>
<script setup lang="ts">
import type { FormInstance, FormRules } from 'element-plus'
import { ref, reactive } from 'vue';
import { requestOverWs } from "@/store/requestOverWs";


const req = requestOverWs();
const ruleFormRef = ref<FormInstance>()
const props = defineProps(['show']);
const loading = ref(false);
const success = ref("");
const fail = ref("");
const form = reactive({
  username: "",
  password: "",
  confirmPassword: ""
});
const submit = async (form: { username: string, password: string, confirmPassword: string }) => {
  loading.value = true;
  fail.value = "";
  success.value = "";
  try {
    const res = await req.query({
      type: "addUser",
      username: form.username,
      password: form.password
    });
    if (!res.status) {
      fail.value = res.msg;
    } else {
      success.value = "添加成功";
    }
  } catch (e: any) {
    loading.value = false;
    fail.value = e.toString();
  }
}

const validatePass = (rule: any, value: any, callback: any) => {
  if (value === '') {
    callback(new Error('请输入密码'))
  } else {
    if (form.confirmPassword !== '') {
      if (!ruleFormRef.value) return
      ruleFormRef.value.validateField('checkPass')
    }
    callback()
  }
}
const validatePass2 = (rule: any, value: any, callback: any) => {
  if (value === '') {
    callback(new Error('请输入密码'))
  } else if (value !== form.password) {
    callback(new Error("密码不匹配"))
  } else {
    callback()
  }
}

const rules = reactive<FormRules<typeof form>>({
  password: [{ validator: validatePass, trigger: 'blur' }],
  confirmPassword: [{ validator: validatePass2, trigger: 'blur' }]
})
</script>