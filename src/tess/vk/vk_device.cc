#include "tess/vk/vk_device.h"

#include <iostream>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// DeviceList
//
DeviceList::DeviceList() = default;

DeviceList::DeviceList(Instance instance)
  : instance_(instance)
{
  // Initialize logical device create info
  device_info_.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info_.pNext = NULL;

  device_info_.queueCreateInfoCount = 1;
  device_info_.pQueueCreateInfos = &queue_info_;

  device_info_.enabledLayerCount = 0;
  device_info_.ppEnabledLayerNames = NULL;

  device_info_.enabledExtensionCount = 0;
  device_info_.ppEnabledExtensionNames = NULL;

  device_info_.pEnabledFeatures = NULL;

  // Query physical devices
  uint32_t physical_device_count;
  vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
  std::vector<VkPhysicalDevice> physical_device_handles(physical_device_count);
  vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_device_handles.data());

  // Create PhysicalDevice wrappers
  for (auto physical_device_handle : physical_device_handles)
  {
    physical_devices_.emplace_back(instance_, physical_device_handle);
    physical_devices_.back().LoadExtensions();
    physical_devices_.back().LoadQueueFamilies();
  }
}

DeviceList::~DeviceList()
{
}

void DeviceList::SetExtensions(const std::vector<const char*>& extensions)
{
  // Not using extension layers
  device_info_.enabledLayerCount = 0;
  device_info_.ppEnabledLayerNames = NULL;

  device_info_.enabledExtensionCount = extensions.size();
  device_info_.ppEnabledExtensionNames = extensions.data();
}

void DeviceList::PrintDeviceExtensionProperties()
{
  std::cout << "Device Extensions" << std::endl
    << "================" << std::endl;

  for (auto physical_device : physical_devices_)
  {
    physical_device.PrintDeviceExtensionProperties();
    std::cout << std::endl;
  }

  /*
  Device Extensions
  ================

  Device 0000012D91FAD990: 104 extensions
                  |---[Device Extension]--> VK_KHR_16bit_storage
                  |---[Device Extension]--> VK_KHR_8bit_storage
                  |---[Device Extension]--> VK_KHR_bind_memory2
                  |---[Device Extension]--> VK_KHR_buffer_device_address
                  |---[Device Extension]--> VK_KHR_create_renderpass2
                  |---[Device Extension]--> VK_KHR_dedicated_allocation
                  |---[Device Extension]--> VK_KHR_depth_stencil_resolve
                  |---[Device Extension]--> VK_KHR_descriptor_update_template
                  |---[Device Extension]--> VK_KHR_device_group
                  |---[Device Extension]--> VK_KHR_draw_indirect_count
                  |---[Device Extension]--> VK_KHR_driver_properties
                  |---[Device Extension]--> VK_KHR_external_fence
                  |---[Device Extension]--> VK_KHR_external_fence_win32
                  |---[Device Extension]--> VK_KHR_external_memory
                  |---[Device Extension]--> VK_KHR_external_memory_win32
                  |---[Device Extension]--> VK_KHR_external_semaphore
                  |---[Device Extension]--> VK_KHR_external_semaphore_win32
                  |---[Device Extension]--> VK_KHR_get_memory_requirements2
                  |---[Device Extension]--> VK_KHR_image_format_list
                  |---[Device Extension]--> VK_KHR_imageless_framebuffer
                  |---[Device Extension]--> VK_KHR_maintenance1
                  |---[Device Extension]--> VK_KHR_maintenance2
                  |---[Device Extension]--> VK_KHR_maintenance3
                  |---[Device Extension]--> VK_KHR_multiview
                  |---[Device Extension]--> VK_KHR_pipeline_executable_properties
                  |---[Device Extension]--> VK_KHR_push_descriptor
                  |---[Device Extension]--> VK_KHR_relaxed_block_layout
                  |---[Device Extension]--> VK_KHR_sampler_mirror_clamp_to_edge
                  |---[Device Extension]--> VK_KHR_sampler_ycbcr_conversion
                  |---[Device Extension]--> VK_KHR_separate_depth_stencil_layouts
                  |---[Device Extension]--> VK_KHR_shader_atomic_int64
                  |---[Device Extension]--> VK_KHR_shader_clock
                  |---[Device Extension]--> VK_KHR_shader_draw_parameters
                  |---[Device Extension]--> VK_KHR_shader_float16_int8
                  |---[Device Extension]--> VK_KHR_shader_float_controls
                  |---[Device Extension]--> VK_KHR_shader_non_semantic_info
                  |---[Device Extension]--> VK_KHR_shader_subgroup_extended_types
                  |---[Device Extension]--> VK_KHR_spirv_1_4
                  |---[Device Extension]--> VK_KHR_storage_buffer_storage_class
                  |---[Device Extension]--> VK_KHR_swapchain
                  |---[Device Extension]--> VK_KHR_swapchain_mutable_format
                  |---[Device Extension]--> VK_KHR_timeline_semaphore
                  |---[Device Extension]--> VK_KHR_uniform_buffer_standard_layout
                  |---[Device Extension]--> VK_KHR_variable_pointers
                  |---[Device Extension]--> VK_KHR_vulkan_memory_model
                  |---[Device Extension]--> VK_KHR_win32_keyed_mutex
                  |---[Device Extension]--> VK_EXT_blend_operation_advanced
                  |---[Device Extension]--> VK_EXT_buffer_device_address
                  |---[Device Extension]--> VK_EXT_calibrated_timestamps
                  |---[Device Extension]--> VK_EXT_conditional_rendering
                  |---[Device Extension]--> VK_EXT_conservative_rasterization
                  |---[Device Extension]--> VK_EXT_custom_border_color
                  |---[Device Extension]--> VK_EXT_depth_clip_enable
                  |---[Device Extension]--> VK_EXT_depth_range_unrestricted
                  |---[Device Extension]--> VK_EXT_descriptor_indexing
                  |---[Device Extension]--> VK_EXT_discard_rectangles
                  |---[Device Extension]--> VK_EXT_external_memory_host
                  |---[Device Extension]--> VK_EXT_fragment_shader_interlock
                  |---[Device Extension]--> VK_EXT_full_screen_exclusive
                  |---[Device Extension]--> VK_EXT_hdr_metadata
                  |---[Device Extension]--> VK_EXT_host_query_reset
                  |---[Device Extension]--> VK_EXT_index_type_uint8
                  |---[Device Extension]--> VK_EXT_inline_uniform_block
                  |---[Device Extension]--> VK_EXT_line_rasterization
                  |---[Device Extension]--> VK_EXT_memory_budget
                  |---[Device Extension]--> VK_EXT_memory_priority
                  |---[Device Extension]--> VK_EXT_pci_bus_info
                  |---[Device Extension]--> VK_EXT_pipeline_creation_feedback
                  |---[Device Extension]--> VK_EXT_post_depth_coverage
                  |---[Device Extension]--> VK_EXT_sample_locations
                  |---[Device Extension]--> VK_EXT_sampler_filter_minmax
                  |---[Device Extension]--> VK_EXT_scalar_block_layout
                  |---[Device Extension]--> VK_EXT_separate_stencil_usage
                  |---[Device Extension]--> VK_EXT_shader_demote_to_helper_invocation
                  |---[Device Extension]--> VK_EXT_shader_subgroup_ballot
                  |---[Device Extension]--> VK_EXT_shader_subgroup_vote
                  |---[Device Extension]--> VK_EXT_shader_viewport_index_layer
                  |---[Device Extension]--> VK_EXT_subgroup_size_control
                  |---[Device Extension]--> VK_EXT_texel_buffer_alignment
                  |---[Device Extension]--> VK_EXT_tooling_info
                  |---[Device Extension]--> VK_EXT_transform_feedback
                  |---[Device Extension]--> VK_EXT_vertex_attribute_divisor
                  |---[Device Extension]--> VK_EXT_ycbcr_image_arrays
                  |---[Device Extension]--> VK_NV_clip_space_w_scaling
                  |---[Device Extension]--> VK_NV_coverage_reduction_mode
                  |---[Device Extension]--> VK_NV_dedicated_allocation
                  |---[Device Extension]--> VK_NV_dedicated_allocation_image_aliasing
                  |---[Device Extension]--> VK_NV_device_diagnostic_checkpoints
                  |---[Device Extension]--> VK_NV_device_diagnostics_config
                  |---[Device Extension]--> VK_NV_external_memory
                  |---[Device Extension]--> VK_NV_external_memory_win32
                  |---[Device Extension]--> VK_NV_fill_rectangle
                  |---[Device Extension]--> VK_NV_fragment_coverage_to_color
                  |---[Device Extension]--> VK_NV_framebuffer_mixed_samples
                  |---[Device Extension]--> VK_NV_geometry_shader_passthrough
                  |---[Device Extension]--> VK_NV_ray_tracing
                  |---[Device Extension]--> VK_NV_sample_mask_override_coverage
                  |---[Device Extension]--> VK_NV_shader_sm_builtins
                  |---[Device Extension]--> VK_NV_shader_subgroup_partitioned
                  |---[Device Extension]--> VK_NV_viewport_array2
                  |---[Device Extension]--> VK_NV_viewport_swizzle
                  |---[Device Extension]--> VK_NV_win32_keyed_mutex
                  |---[Device Extension]--> VK_NVX_device_generated_commands
                  |---[Device Extension]--> VK_NVX_multiview_per_view_attributes
  */
}

void DeviceList::PrintDeviceQueueFamilies()
{
  std::cout << "Device Queue Families" << std::endl
    << "================" << std::endl;

  for (auto physical_device : physical_devices_)
  {
    physical_device.PrintDeviceQueueFamilies();
    std::cout << std::endl;
  }
}

PhysicalDevice DeviceList::SelectGraphicsDevice(int device_index)
{
  return PhysicalDevice(physical_devices_[device_index]);
}

PhysicalDevice DeviceList::SelectBestGraphicsDevice()
{
  // TODO: select the best device
  // Assuming there is only one GPU, return 0 index device
  return SelectGraphicsDevice(0);
}


//
// PhysicalDevice
//
PhysicalDevice::PhysicalDevice() = default;

PhysicalDevice::PhysicalDevice(Instance instance, VkPhysicalDevice physical_device)
  : instance_(instance), physical_device_(physical_device)
{
}

PhysicalDevice::~PhysicalDevice()
{
}

void PhysicalDevice::LoadExtensions()
{
  VkResult result;

  // Load device extensions
  uint32_t extension_count;
  result = vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &extension_count, NULL);
  extensions_.resize(extension_count);
  result = vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &extension_count, extensions_.data());

  // Register names
  for (const auto& extension_property : extensions_)
    extension_names_.insert(extension_property.extensionName);
}

void PhysicalDevice::PrintDeviceExtensionProperties()
{
  std::cout << "Device " << physical_device_ << ": " << extensions_.size() << " extensions" << std::endl
    << "==============" << std::endl;

  for (const auto& extension : extensions_)
    std::cout << "\t|---[Device Extension]--> " << extension.extensionName << std::endl;

  // Layers are deprecated in Vulkan 1.1 and higher version
  /*
  Device Layers
  ==============
  NVIDIA Optimus layer
          |---[Layer Name]--> VK_LAYER_NV_optimus

  Open Broadcaster Software hook
          |---[Layer Name]--> VK_LAYER_OBS_HOOK

  Khronos Validation Layer
          |---[Layer Name]--> VK_LAYER_KHRONOS_validation
                  |---[Layer Extension]--> VK_EXT_validation_cache
                  |---[Layer Extension]--> VK_EXT_debug_marker
                  |---[Layer Extension]--> VK_EXT_tooling_info
  */
}

void PhysicalDevice::LoadQueueFamilies()
{
  uint32_t queue_family_property_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_property_count, NULL);
  queue_families_.resize(queue_family_property_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_property_count, queue_families_.data());
}

void PhysicalDevice::PrintDeviceQueueFamilies()
{
  std::cout << "Device " << physical_device_ << ": " << queue_families_.size() << " families" << std::endl;

  for (const auto& queue_family : queue_families_)
  {
    std::cout
      << "\t(Graphics " << !!(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) << ")" << std::endl
      << "\t(Compute  " << !!(queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) << ")" << std::endl
      << "\t(Transfer " << !!(queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) << ")" << std::endl
      << "\t(Sparse   " << !!(queue_family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) << ")" << std::endl
      << "\tQueue count: " << queue_family.queueCount << std::endl
      << std::endl;
  }
}

bool PhysicalDevice::IsSupportedExtension(const std::string& extension_name)
{
  return extension_names_.find(extension_name) != extension_names_.cend();
}


//
// DeviceCreator
//
DeviceCreator::DeviceCreator(PhysicalDevice physical_device)
  : physical_device_(physical_device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info_.pNext = NULL;

  create_info_.flags = 0;
  create_info_.pEnabledFeatures = &physical_device_features_;

  queue_family_count_.resize(physical_device.QueueFamilies().size(), 0);
}

DeviceCreator::~DeviceCreator()
{
}

void DeviceCreator::AddExtension(DeviceExtension extension)
{
  AddExtension(LayerExtension::ExtensionName(extension));
}

void DeviceCreator::AddExtension(const std::string& extension_name)
{
  if (physical_device_.IsSupportedExtension(extension_name))
  {
    // Check duplication
    bool found = false;
    for (const auto& extension : extensions_)
    {
      if (extension == extension_name)
      {
        found = true;
        break;
      }
    }

    if (!found)
      extensions_.push_back(extension_name);
  }
  else
    throw std::runtime_error("Unsupported extension: \"" + extension_name + "\"!");
}

void DeviceCreator::AddGraphicsQueue()
{
  const auto& queue_families = physical_device_.QueueFamilies();

  bool found = false;
  for (int i = 0; i < queue_families.size(); i++)
  {
    const auto& queue_family = queue_families[i];

    if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
      queue_family_count_[i] < queue_family.queueCount)
    {
      found = true;
      queue_family_count_[i]++;
      break;
    }
  }

  if (!found)
    throw std::runtime_error("Cannot add a graphics queue any more!");
}

Device DeviceCreator::Create()
{
  int queue_max_count = 0;
  for (int i = 0; i < queue_family_count_.size(); i++)
  {
    if (queue_max_count < queue_family_count_[i])
      queue_max_count = queue_family_count_[i];
  }

  // Populate queue create info
  std::vector<VkDeviceQueueCreateInfo> queue_infos;
  std::vector<float> queue_priority(queue_max_count, 1.f);
  if (queue_max_count)
  {
    for (int i = 0; i < queue_family_count_.size(); i++)
    {
      if (queue_family_count_[i] > 0)
      {
        queue_infos.emplace_back();
        auto& queue_info = queue_infos.back();

        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.pNext = NULL;
        queue_info.flags = 0;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = queue_family_count_[i];
        queue_info.pQueuePriorities = queue_priority.data();
      }
    }
  }

  // Queue create info
  if (!queue_infos.empty())
  {
    create_info_.queueCreateInfoCount = queue_infos.size();
    create_info_.pQueueCreateInfos = queue_infos.data();
  }
  else
  {
    create_info_.queueCreateInfoCount = 0;
    create_info_.pQueueCreateInfos = NULL;
  }

  // Device layers are deprecated
  create_info_.enabledLayerCount = 0;
  create_info_.ppEnabledLayerNames = NULL;

  std::vector<const char*> extension_names;
  for (const auto& extension : extensions_)
    extension_names.push_back(extension.c_str());

  create_info_.enabledExtensionCount = extension_names.size();
  create_info_.ppEnabledExtensionNames = extension_names.data();

  Device device;

  VkResult result;
  result = vkCreateDevice(physical_device_, &create_info_, NULL, &device.device_);

  // Retrieve queue handles
  for (int i = 0; i < queue_family_count_.size(); i++)
  {
    for (int j = 0; j < queue_family_count_[i]; j++)
    {
      device.queues_.emplace_back();
      vkGetDeviceQueue(device, i, j, &device.queues_.back().queue_);
    }
  }

  return device;
}


//
// Device
//
Device::Device() = default;

Device::Device(Instance instance)
  : instance_(instance)
{
}

Device::~Device()
{
}

void Device::Destroy()
{
  if (device_)
  {
    vkDestroyDevice(device_, NULL);
    device_ = NULL;
  }
}

void Device::WaitIdle()
{
  vkDeviceWaitIdle(device_);
}
}
}
