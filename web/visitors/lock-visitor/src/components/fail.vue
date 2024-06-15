<template>
    <el-dialog v-model="props.show" title="错误" width="500" align-center :close-on-click-modal="false"
        :close-on-press-escape="false" :show-close="false">
        <b>{{ props.reason }}</b>
        <template #footer>
            <div class="dialog-footer" style="text-align: center;">
                <el-button type="primary" @click="reload">
                    重新加载
                </el-button>
            </div>
        </template>
    </el-dialog>
</template>
<script setup lang="ts">
const props = defineProps(['show', 'reason']);
function reload(form: { username: string, password: string }) {
    location.reload();
}
</script>