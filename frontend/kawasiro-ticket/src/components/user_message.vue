<template>
	<v-container>
		<v-card class="mx-auto" max-width="434">

			<v-img height="100%" src="https://cdn.vuetifyjs.com/images/cards/server-room.jpg">
				<v-container>
					<div class="text-center">
						<v-row align="end" class="fill-height">
							<v-col align-self="start" class="pa-0" cols="12">
								<v-avatar color="orange" size="100">
									<span class="white--text headline">62</span>
								</v-avatar>
							</v-col>
							<v-col class="py-0">
								<v-list-item color="rgba(0, 0, 0, .4)" dark>
									<v-list-item-content>
										<v-list-item-title class="title">大湿</v-list-item-title>
										<v-list-item-subtitle>已登录</v-list-item-subtitle>
									</v-list-item-content>
								</v-list-item>
							</v-col>
						</v-row>
					</div>
				</v-container>
			</v-img>
			<v-container fluid>
				<small>用户信息</small>
				<v-row align="center">
					<v-col cols="12">
						<v-card-text v-if="isEditing">
							<v-text-field prepend-icon="mdi-city" :disabled="!isEditing" color="black" prefix="昵称" dense outlined></v-text-field>
							<v-text-field prepend-icon="mdi-city" :disabled="!isEditing" color="black" prefix="邮箱" dense outlined></v-text-field>
							<v-text-field prepend-icon="mdi-city" :disabled="!isEditing" color="black" prefix="QQ" dense outlined></v-text-field>
						</v-card-text>
						<v-card-text v-else>
							<v-col>
								<v-card class="d-flex align-center justify-sm-space-between pa-2 mx-auto" max-width="550" outlined>
									<div>账号:</div>
									<div>18829993098</div>
								</v-card>
							</v-col>

							<v-col>
								<v-card class="d-flex align-center justify-sm-space-between pa-2 mx-auto" max-width="550" outlined>
									<div>昵称:</div>
									<div>大湿</div>
								</v-card>
							</v-col>

							<v-col>
								<v-card class="d-flex align-center justify-sm-space-between pa-2 mx-auto" max-width="550" outlined>
									<div>邮箱:</div>
									<div>example@xx.com</div>
								</v-card>
							</v-col>

							<v-col>
								<v-card class="d-flex align-center justify-sm-space-between pa-2 mx-auto" max-width="550" outlined>
									<div>qq:</div>
									<div>9999999999</div>
								</v-card>
							</v-col>

						</v-card-text>
						<v-card-action>
							<v-col class="d-flex justify-sm-space-between flex-row">
								<v-btn color="blue" fab small @click="isEditing = !isEditing">
									<v-icon v-if="isEditing">mdi-close</v-icon>
									<v-icon v-else>mdi-pencil</v-icon>
								</v-btn>
								<v-spacer></v-spacer>
								<v-btn :disabled="autoUpdate" :loading="isUpdating" color="blue-grey" depressed @click="isUpdating = true" />
							</v-col>
						</v-card-action>
					</v-col>
				</v-row>
			</v-container>
		</v-card>
	</v-container>
</template>

<script>
	export default {
		name: 'user_message',

		data: () => ({
			isEditing: null,
		}),

		methods: {
			save() {
				this.isEditing = !this.isEditing
				this.hasSaved = true
			},
		},
	}
</script>
