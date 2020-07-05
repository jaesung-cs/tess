#include "tess/vk/vk_pipeline.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// GraphicsPipelineCreator
//
GraphicsPipelineCreator::GraphicsPipelineCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info_.pVertexInputState = &vertex_input_info_;
  create_info_.pInputAssemblyState = &input_assembly_;
  create_info_.pViewportState = &viewport_state_;
  create_info_.pRasterizationState = &rasterizer_;
  create_info_.pMultisampleState = &multisampling_;
  create_info_.pColorBlendState = &color_blending_;

  // TODO: enable viewport dynamic state
  //create_info_.pDynamicState = &dynamic_state_;

  create_info_.subpass = 0;

  // Fixed graphics pipeline functions
  vertex_input_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info_.vertexAttributeDescriptionCount = 0;
  vertex_input_info_.vertexBindingDescriptionCount = 0;

  binding_description_.binding = 0;
  binding_description_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  input_assembly_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_.primitiveRestartEnable = VK_FALSE;

  viewport_.minDepth = 0.f;
  viewport_.maxDepth = 1.f;

  scissor_.offset = { 0, 0 };

  viewport_state_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_.viewportCount = 1;
  viewport_state_.pViewports = &viewport_;
  viewport_state_.scissorCount = 1;
  viewport_state_.pScissors = &scissor_;

  rasterizer_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_.depthClampEnable = VK_FALSE;
  rasterizer_.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_.lineWidth = 1.0f;
  rasterizer_.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer_.depthBiasEnable = VK_FALSE;

  multisampling_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_.sampleShadingEnable = VK_FALSE;
  multisampling_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  color_blend_attachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment_.blendEnable = VK_TRUE;
  color_blend_attachment_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment_.alphaBlendOp = VK_BLEND_OP_ADD;

  color_blending_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending_.logicOpEnable = VK_FALSE;
  color_blending_.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending_.attachmentCount = 1;
  color_blending_.pAttachments = &color_blend_attachment_;

  dynamic_states_.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  dynamic_states_.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);

  dynamic_state_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_.dynamicStateCount = dynamic_states_.size();
  dynamic_state_.pDynamicStates = dynamic_states_.data();
}

GraphicsPipelineCreator::~GraphicsPipelineCreator()
{
}

void GraphicsPipelineCreator::SetAttributeStride(int stride)
{
  binding_description_.stride = stride;
}

void GraphicsPipelineCreator::SetAttribute(int location, int size, int offset)
{
  attribute_descriptions_.emplace_back();
  auto& attribute_description = attribute_descriptions_.back();

  attribute_description.binding = 0;
  attribute_description.location = location;

  switch (size)
  {
  case 1: attribute_description.format = VK_FORMAT_R32_SFLOAT; break;
  case 2: attribute_description.format = VK_FORMAT_R32G32_SFLOAT; break;
  case 3: attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT; break;
  case 4: attribute_description.format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
  }

  attribute_description.offset = offset;
}

void GraphicsPipelineCreator::AddVertexShaderStage(ShaderModule shader_module)
{
  shader_stages_.push_back({});
  auto& stage = shader_stages_.back();

  stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  stage.module = shader_module;
  stage.pName = "main";
}

void GraphicsPipelineCreator::AddFragmentShaderStage(ShaderModule shader_module)
{
  shader_stages_.push_back({});
  auto& stage = shader_stages_.back();

  stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  stage.module = shader_module;
  stage.pName = "main";
}

void GraphicsPipelineCreator::SetViewport(float width, float height)
{
  SetViewport(0, 0, width, height);
}

void GraphicsPipelineCreator::SetViewport(float x, float y, float width, float height)
{
  viewport_.x = x;
  viewport_.y = y;
  viewport_.width = width;
  viewport_.height = height;

  scissor_.extent.width = width;
  scissor_.extent.height = height;
}

void GraphicsPipelineCreator::SetPipelineLayout(PipelineLayout pipeline_layout)
{
  create_info_.layout = pipeline_layout;
}

void GraphicsPipelineCreator::SetRenderPass(RenderPass render_pass)
{
  create_info_.renderPass = render_pass;
}

Pipeline GraphicsPipelineCreator::Create()
{
  if (!attribute_descriptions_.empty())
  {
    vertex_input_info_.vertexBindingDescriptionCount = 1;
    vertex_input_info_.pVertexBindingDescriptions = &binding_description_;

    vertex_input_info_.vertexAttributeDescriptionCount = attribute_descriptions_.size();
    vertex_input_info_.pVertexAttributeDescriptions = attribute_descriptions_.data();
  }

  create_info_.stageCount = shader_stages_.size();
  create_info_.pStages = shader_stages_.data();

  Pipeline pipeline{ device_ };

  if (VkResult result = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &create_info_, NULL, &pipeline.pipeline_))
    throw Exception("Failed to create a graphics pipeline!", result);

  return pipeline;
}


//
// Pipeline
//
Pipeline::Pipeline()
{
}

Pipeline::Pipeline(Device device)
  : device_(device)
{
}

Pipeline::~Pipeline()
{
}

void Pipeline::Destroy()
{
  if (pipeline_)
  {
    vkDestroyPipeline(device_, pipeline_, NULL);
    pipeline_ = NULL;
  }
}
}
}
