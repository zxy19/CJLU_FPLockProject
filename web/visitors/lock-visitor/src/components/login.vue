<template>
    <el-dialog v-model="props.show" title="登录" width="500" align-center :close-on-click-modal="false"
        :close-on-press-escape="false" :show-close="false">
        <el-form label-position="top" label-width="auto" :model="form" style="max-width: 600px">
            <el-form-item label="用户名">
                <el-input v-model="form.username" />
            </el-form-item>
            <el-form-item label="密码">
                <el-input v-model="form.password" />
            </el-form-item>
            <el-form-item>
                <el-button type="primary" @click="submit(form)">
                    提交
                </el-button>
            </el-form-item>
        </el-form>
    </el-dialog>
</template>
<script setup lang="ts">
import { ref } from 'vue';

const props = defineProps(['show']);
const emit = defineEmits(["login"]);
const form = ref({
    username: "",
    password: ""
});
function submit(form: { username: string, password: string }) {
    emit("login", form);
}
</script>