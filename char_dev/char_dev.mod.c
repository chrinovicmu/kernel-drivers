#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x92997ed8, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xf4976bc5, "module_put" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x6340598d, "try_module_get" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa8b1ed91, "cdev_init" },
	{ 0x6c34b9d1, "cdev_add" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x383b0ba, "__class_create" },
	{ 0xbc633891, "cdev_del" },
	{ 0x8cd8e4c3, "device_create" },
	{ 0xf6f5e018, "class_destroy" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xacd987ad, "device_destroy" },
	{ 0x5219c37a, "module_layout" },
};

MODULE_INFO(depends, "");

