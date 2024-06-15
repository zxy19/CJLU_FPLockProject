<template>
    <el-drawer v-model="props.show" title="用户管理" direction="rtl" size="70%">
        <el-button type="primary" @click="showAddUser = true">开门</el-button>
        <addUserVue :show="showAddUser" @add="addUser"></addUserVue>
        <el-table :data="props.users">
            <el-table-column property="username" label="用户名" width="150" />
            <el-table-column>
                <template #default="scope">
                    <el-button link type="primary" size="small" @click.prevent="remove(scope.$index)">
                        移除用户
                    </el-button>
                </template>
            </el-table-column>
        </el-table>
    </el-drawer>
</template>
<script setup lang="ts">
import { ref } from 'vue';
import addUserVue from './addUser.vue';
const showAddUser = ref(false);
const props: {
    show?: boolean,
    users?: { username: string }[]
} = defineProps(['show', 'users']);

const emit = defineEmits(['delete', 'add']);

const show = ref(props.show);

const remove = (id: number) => {
    emit('delete', id);
}
const addUser = (data) => {
    emit('add', data);
}
</script>