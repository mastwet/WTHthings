<template>
	<v-container class="fill-height" fluid>
		<v-card class="elevation-12">
			<v-toolbar  color="primary" dark flat>
				<v-toolbar-title>登录</v-toolbar-title>
				<v-spacer></v-spacer>
				<v-tooltip bottom>
					<template v-slot:activator="{ on }">
						<v-btn icon large target="_blank" v-on="on">
							<v-icon>mdi-code-tags</v-icon>
						</v-btn>
					</template>
				</v-tooltip>
			</v-toolbar>
			<v-card-text>
				<v-form>
					<v-row>
						<v-col>
							<v-text-field label="手机" name="login" prepend-icon="mdi-account" type="text"></v-text-field>
						</v-col>
					</v-row>
					<v-row>
						<v-col :cols="7">
							<v-text-field id="password" label="验证码" name="password" prepend-icon="mdi-lock" type="password"></v-text-field>
						</v-col>
						<v-col :cols="3" style="padding-top:25px;">
							<v-btn color="primary" style="font-size: 3px;" outlined>获取验证码</v-btn>
						</v-col>
					</v-row>
				</v-form>
			</v-card-text>
			<v-card-actions>
				<v-spacer></v-spacer>
					<v-btn color="primary">登录</v-btn>
			</v-card-actions>
		</v-card>
	</v-container>
</template>

<script>
	export default {
		name: 'login',
		props: {
			source: String,
		},
		data: () => ({

		})
	}
</script>

<style>
</style>
