local ffi = require('ffi')
ffi.cdef[[
	typedef struct YGNode* YGNodeRef;
	typedef const struct YGNode* YGNodeConstRef;
	typedef enum { YGEdgeLeft, YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeStart, YGEdgeEnd, YGEdgeHorizontal, YGEdgeVertical, YGEdgeAll } YGEdge;
	typedef enum { YGDirectionInherit, YGDirectionLTR, YGDirectionRTL } YGDirection;
	typedef enum { YGFlexDirectionColumn, YGFlexDirectionColumnReverse, YGFlexDirectionRow, YGFlexDirectionRowReverse } YGFlexDirection;
	typedef enum { YGJustifyFlexStart, YGJustifyCenter, YGJustifyFlexEnd, YGJustifySpaceBetween, YGJustifySpaceAround, YGJustifySpaceEvenly	} YGJustify;
	typedef enum { YGAlignAuto, YGAlignFlexStart, YGAlignCenter, YGAlignFlexEnd, YGAlignStretch, YGAlignBaseline, YGAlignSpaceBetween, YGAlignSpaceAround } YGAlign;
	typedef enum { YGPositionTypeStatic, YGPositionTypeRelative, YGPositionTypeAbsolute } YGPositionType;
	typedef enum { YGWrapNoWrap, YGWrapWrap, YGWrapWrapReverse } YGWrap;
	typedef enum { YGOverflowVisible, YGOverflowHidden, YGOverflowScroll } YGOverflow;
	typedef enum { YGUnitUndefined, YGUnitPoint, YGUnitPercent, YGUnitAuto } YGUnit;
	typedef struct { float value; YGUnit unit; } YGValue;

	/* disable creating nodes without context, use the registered LUA_YGNodeNew() instead */
	/* YGNodeRef YGNodeNew(); */
	YGNodeRef YGNodeClone(YGNodeRef node);
	void YGNodeFree(YGNodeRef node);
	void FFI_CleanupYogaNode(YGNodeRef node);
	void FFI_YGNodeFreeRecursiveWithCleanupFunc(YGNodeRef node); // replaces YGNodeFreeRecursive()
	void YGNodeReset(YGNodeRef node);

	void* YGNodeGetContext(YGNodeRef node);
	void YGNodeSetContext(YGNodeRef node, void* context);

	void YGNodeStyleSetWidth(YGNodeRef node, float width);
	void YGNodeStyleSetWidthPercent(YGNodeRef node, float width);
	void YGNodeStyleSetWidthAuto(YGNodeRef node);
	void YGNodeStyleSetMinWidth(YGNodeRef node, float minWidth);
	void YGNodeStyleSetMinWidthPercent(YGNodeRef node, float minWidth);
	void YGNodeStyleSetMaxWidth(YGNodeRef node, float maxWidth);
	void YGNodeStyleSetMaxWidthPercent(YGNodeRef node, float maxWidth);

	void YGNodeStyleSetHeight(YGNodeRef node, float height);
	void YGNodeStyleSetHeightPercent(YGNodeRef node, float height);
	void YGNodeStyleSetHeightAuto(YGNodeRef node);
	void YGNodeStyleSetMinHeight(YGNodeRef node, float minHeight);
	void YGNodeStyleSetMinHeightPercent(YGNodeRef node, float minHeight);
	void YGNodeStyleSetMaxHeight(YGNodeRef node, float maxHeight);
	void YGNodeStyleSetMaxHeightPercent(YGNodeRef node, float maxHeight);

	YGValue YGNodeStyleGetWidth(YGNodeConstRef node);
	YGValue YGNodeStyleGetHeight(YGNodeConstRef node);
	YGValue YGNodeStyleGetMinWidth(YGNodeConstRef node);
	YGValue YGNodeStyleGetMinHeight(YGNodeConstRef node);
	YGValue YGNodeStyleGetMaxWidth(YGNodeConstRef node);
	YGValue YGNodeStyleGetMaxHeight(YGNodeConstRef node);

	void YGNodeStyleSetFlexDirection(YGNodeRef node, YGFlexDirection flexDirection);
	YGFlexDirection YGNodeStyleGetFlexDirection(YGNodeConstRef node);
	
	void YGNodeStyleSetJustifyContent(YGNodeRef node, YGJustify justifyContent);
	YGJustify YGNodeStyleGetJustifyContent(YGNodeConstRef node);

	void YGNodeStyleSetAlignContent(YGNodeRef node, YGAlign alignContent);
	YGAlign YGNodeStyleGetAlignContent(YGNodeConstRef node);

	void YGNodeStyleSetAlignItems(YGNodeRef node, YGAlign alignItems);
	YGAlign YGNodeStyleGetAlignItems(YGNodeConstRef node);

	void YGNodeStyleSetAlignSelf(YGNodeRef node, YGAlign alignSelf);
	YGAlign YGNodeStyleGetAlignSelf(YGNodeConstRef node);

	void YGNodeStyleSetPositionType(YGNodeRef node, YGPositionType positionType);
	YGPositionType YGNodeStyleGetPositionType(YGNodeConstRef node);

	void YGNodeStyleSetFlexWrap(YGNodeRef node, YGWrap flexWrap);
	YGWrap YGNodeStyleGetFlexWrap(YGNodeConstRef node);

	void YGNodeStyleSetOverflow(YGNodeRef node, YGOverflow overflow);
	YGOverflow YGNodeStyleGetOverflow(YGNodeConstRef node);

	void YGNodeStyleSetFlex(YGNodeRef node, float flex);
	float YGNodeStyleGetFlex(YGNodeConstRef node);
	
	void YGNodeStyleSetFlexGrow(YGNodeRef node, float flexGrow);
	float YGNodeStyleGetFlexGrow(YGNodeConstRef node);
	
	void YGNodeStyleSetFlexShrink(YGNodeRef node, float flexShrink);
	float YGNodeStyleGetFlexShrink(YGNodeConstRef node);

	void YGNodeStyleSetPosition(YGNodeRef node, YGEdge edge, float position);
	void YGNodeStyleSetPositionPercent(YGNodeRef node, YGEdge edge, float position);
	YGValue YGNodeStyleGetPosition(YGNodeConstRef node, YGEdge edge);

	void YGNodeStyleSetAspectRatio(YGNodeRef node, float aspectRatio);
	float YGNodeStyleGetAspectRatio(YGNodeConstRef node);

	float YGNodeLayoutGetLeft(YGNodeConstRef node);
	float YGNodeLayoutGetTop(YGNodeConstRef node);
	float YGNodeLayoutGetRight(YGNodeConstRef node);
	float YGNodeLayoutGetBottom(YGNodeConstRef node);
	float YGNodeLayoutGetWidth(YGNodeConstRef node);
	float YGNodeLayoutGetHeight(YGNodeConstRef node);
	bool YGNodeLayoutGetHadOverflow(YGNodeRef node);

	void YGNodeStyleSetMargin(YGNodeRef node, YGEdge edge, float margin);
	void YGNodeStyleSetMarginPercent(YGNodeRef node, YGEdge edge, float margin);
	void YGNodeStyleSetMarginAuto(YGNodeRef node, YGEdge edge);

	void YGNodeStyleSetPadding(YGNodeRef node, YGEdge edge, float padding);
	void YGNodeStyleSetPaddingPercent(YGNodeRef node, YGEdge edge, float padding);

	void YGNodeStyleSetBorder(YGNodeRef node, YGEdge edge, float border);

	float YGNodeLayoutGetMargin(YGNodeRef node, YGEdge edge);
	float YGNodeLayoutGetBorder(YGNodeRef node, YGEdge edge);
	float YGNodeLayoutGetPadding(YGNodeRef node, YGEdge edge);

	void YGNodeCopyStyle(YGNodeRef dstNode, YGNodeRef srcNode);

	void YGNodeInsertChild(YGNodeRef node, YGNodeRef child, int index);
	void YGNodeRemoveChild(YGNodeRef node, YGNodeRef child);
	void YGNodeRemoveAllChildren(YGNodeRef node);
	YGNodeRef YGNodeGetChild(YGNodeRef node, int index);
	YGNodeRef YGNodeGetOwner(YGNodeRef node);
	YGNodeRef YGNodeGetParent(YGNodeRef node);
	unsigned int YGNodeGetChildCount(YGNodeRef node);

	void YGNodeCalculateLayout(YGNodeRef node, float availableWidth, float availableHeight, YGDirection ownerDirection);

]]

local Yoga = {}
Yoga.__index = Yoga


Yoga.enums = {
	-- YGEdge
	edge = {
		['left']       = ffi.C.YGEdgeLeft,
		['top']        = ffi.C.YGEdgeTop,
		['right']      = ffi.C.YGEdgeRight,
		['bottom']     = ffi.C.YGEdgeBottom,
		['start']      = ffi.C.YGEdgeStart,
		['end']        = ffi.C.YGEdgeEnd,
		['horizontal'] = ffi.C.YGEdgeHorizontal,
		['vertical']   = ffi.C.YGEdgeVertical,
		['all']        = ffi.C.YGEdgeAll,
	},
	-- YGDirection
	direction = {
		['inherit'] = ffi.C.YGDirectionInherit,
		['ltr']     = ffi.C.YGDirectionLTR,
		['rtl']     = ffi.C.YGDirectionRTL,
	},
	-- YGFlexDirection
	flexdirection = {
		['column']         = ffi.C.YGFlexDirectionColumn,
		['column-reverse'] = ffi.C.YGFlexDirectionColumnReverse,
		['row']            = ffi.C.YGFlexDirectionRow,
		['row-reverse']    = ffi.C.YGFlexDirectionRowReverse,
	},
	-- YGJustify
	justify = {
		['flex-start']    = ffi.C.YGJustifyFlexStart,
		['center']        = ffi.C.YGJustifyCenter,
		['flex-end']      = ffi.C.YGJustifyFlexEnd,
		['space-between'] = ffi.C.YGJustifySpaceBetween,
		['space-around']  = ffi.C.YGJustifySpaceAround,
		['space-evenly']  = ffi.C.YGJustifySpaceEvenly,
	},
	-- YGAlign
	align = {
		['auto']          = ffi.C.YGAlignAuto,
		['flex-start']    = ffi.C.YGAlignFlexStart,
		['center']        = ffi.C.YGAlignCenter,
		['flex-end']      = ffi.C.YGAlignFlexEnd,
		['stretch']       = ffi.C.YGAlignStretch,
		['baseline']      = ffi.C.YGAlignBaseline,
		['space-between'] = ffi.C.YGAlignSpaceBetween,
		['space-around']  = ffi.C.YGAlignSpaceAround,
	},
	-- YGPositionType
	position = {
		['static']   = ffi.C.YGPositionTypeStatic,
		['relative'] = ffi.C.YGPositionTypeRelative,
		['absolute'] = ffi.C.YGPositionTypeAbsolute,
	},
	-- YGWrap
	wrap = {
		['no-wrap']      = ffi.C.YGWrapNoWrap,
		['wrap']         = ffi.C.YGWrapWrap,
		['wrap-reverse'] = ffi.C.YGWrapWrapReverse,
	},
	-- YGOverflow
	overflow = {
		['visible'] = ffi.C.YGOverflowVisible,
		['hidden']  = ffi.C.YGOverflowHidden,
		['scroll']  = ffi.C.YGOverflowScroll,
	},
	-- YGUnit
	unit = {
		['undefined'] = ffi.C.YGUnitUndefined,
		['point']     = ffi.C.YGUnitPoint,
		['percent']   = ffi.C.YGUnitPercent,
		['auto']      = ffi.C.YGUnitAuto,
	},
}

function Yoga.parse(layout, name)
	local root = Yoga.new(nil, name)

	for prop, value in pairs(layout) do
		if type(value) == 'table' then
			--print('<' .. prop .. '>')
			local child = Yoga.parse(value, prop)
			root:insertchild(child)
			--print('</' .. prop .. '>')
		else
			local assignable = Yoga['set_' .. prop]
			if prop == 'position' then
				-- Edge case to stay close to the HTML/CSS definition "position: absolute;"
				Yoga.set_positiontype(root, value)
			elseif prop == 'left' or prop == 'right' or prop == 'top' or prop == 'bottom' or prop == 'horizontal' or prop == 'vertical' or prop == 'start' or prop == 'end' or  prop == 'all' then
				-- Also to stay close to HTML/CSS...
				--print('\x1b[33m' .. prop .. '\x1b[0m' .. ': ' .. value)
				Yoga.set_position(root, value, prop)
			elseif type(assignable) == 'function' then
				-- Anything directly assignable
				--print('\x1b[32m' .. prop .. '\x1b[0m' .. ': ' .. value)
				assignable(root, value)
			elseif string.sub(prop, 0, 1) == '_' then
				-- Ignore props with underscore. (maybe do something with it later)
				--print('\x1b[36m' .. prop .. '\x1b[0m' .. ': ' .. value)
			else
				-- Everything else is an error!
				error('Property "' .. prop .. '" does not exist!')
			end
		end
	end

	return root
end

function Yoga.new(node, name)
	local self = node and { super = node } or { super = LUA_YGNodeNew(name) }
	return setmetatable(self, Yoga)
end

function Yoga:clone()
	return Yoga.new(ffi.C.YGNodeClone(self.super))
end

function Yoga:free(recursive)
	if recursive == nil then recursive = true end
	
	if recursive then
		ffi.C.FFI_YGNodeFreeRecursiveWithCleanupFunc(self.super)
	else
		ffi.C.FFI_CleanupYogaNode(self.super)
		ffi.C.YGNodeFree(self.super)
	end
end

function Yoga:reset()
	ffi.C.YGNodeReset(self.super)
end

--- Assign any context (usually called "userdata") to the node.
-- @param userdata void*
function Yoga:set_context(userdata)
	ffi.C.YGNodeSetContext(self.super, userdata);
	return self
end

--- Returns the nodes context ("userdata").
-- @returns void*
function Yoga:get_context()
	return ffi.C.YGNodeGetContext(self.super);
end

function Yoga:copystyle(target)
	ffi.C.YGNodeCopyStyle(target.super, self.super)
	return self
end

function Yoga:set_width(width)
	if width == 'auto' then
		ffi.C.YGNodeStyleSetWidthAuto(self.super)
	elseif string.sub(width, -1) == '%' then
		width = tonumber(string.sub(width, 0, -2))
		ffi.C.YGNodeStyleSetWidthPercent(self.super, width)
	else
		width = tonumber(width)
		ffi.C.YGNodeStyleSetWidth(self.super, width)
	end

	return self
end

function Yoga:set_minwidth(width)
	if string.sub(width, -1) == '%' then
		width = tonumber(string.sub(width, 0, -2))
		ffi.C.YGNodeStyleSetMinWidthPercent(self.super, width);
	else
		ffi.C.YGNodeStyleSetMinWidth(self.super, width);
	end

	return self
end

function Yoga:set_maxwidth(width)
	if string.sub(width, -1) == '%' then
		width = tonumber(string.sub(width, 0, -2))
		ffi.C.YGNodeStyleSetMaxWidthPercent(self.super, width);
	else
		ffi.C.YGNodeStyleSetMaxWidth(self.super, width);
	end

	return self
end

function Yoga:set_height(height)
	if height == 'auto' then
		ffi.C.YGNodeStyleSetHeightAuto(self.super)
	elseif string.sub(height, -1) == '%' then
		height = tonumber(string.sub(height, 0, -2))
		ffi.C.YGNodeStyleSetHeightPercent(self.super, height)
	else
		height = tonumber(height)
		ffi.C.YGNodeStyleSetHeight(self.super, height)
	end

	return self
end

function Yoga:set_minheight(height)
	if string.sub(height, -1) == '%' then
		height = tonumber(string.sub(height, 0, -2))
		ffi.C.YGNodeStyleSetMinHeightPercent(self.super, height);
	else
		ffi.C.YGNodeStyleSetMinHeight(self.super, height);
	end

	return self
end

function Yoga:set_maxheight(height)
	if string.sub(height, -1) == '%' then
		height = tonumber(string.sub(height, 0, -2))
		ffi.C.YGNodeStyleSetMaxHeightPercent(self.super, height);
	else
		ffi.C.YGNodeStyleSetMaxHeight(self.super, height);
	end

	return self
end

function Yoga:get_width()
	local obj = ffi.C.YGNodeStyleGetWidth(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_minwidth()
	local obj = ffi.C.YGNodeStyleGetMinWidth(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_maxwidth()
	local obj = ffi.C.YGNodeStyleGetMaxWidth(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_height()
	local obj = ffi.C.YGNodeStyleGetHeight(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_minheight()
	local obj = ffi.C.YGNodeStyleGetMinHeight(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_maxheight()
	local obj = ffi.C.YGNodeStyleGetMaxHeight(self.super)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

--- Set the flex direction attribute.
-- @param direction ['column', 'row', 'column-reverse', 'row-reverse']
-- @returns self
function Yoga:set_flexdirection(direction)
	direction = Yoga.enums.flexdirection[direction]
	ffi.C.YGNodeStyleSetFlexDirection(self.super, direction)
	return self
end

function Yoga:get_flexdirection()
	return ffi.C.YGNodeStyleGetFlexDirection(self.super)
end

--- Sets the justify content attribute.
-- @param justify ['flex-start', 'center' , 'flex-end', 'space-between', 'space-around', 'space-evenly']
-- @returns self
function Yoga:set_justifycontent(justify)
	justify = Yoga.enums.justify[justify]
	ffi.C.YGNodeStyleSetJustifyContent(self.super, justify)
	return self
end

function Yoga:get_justifycontent()
	return ffi.C.YGNodeStyleGetJustifyContent(self.super)
end

--- Sets the align content attribute.
-- @param align ['auto', 'flex-start', 'center', 'flex-end', 'stretch', 'baseline', 'space-between', 'space-around']
-- @returns self
function Yoga:set_aligncontent(align)
	align = Yoga.enums.align[align]
	ffi.C.YGNodeStyleSetAlignContent(self.super, align)
	return self
end

function Yoga:get_aligncontent()
	return ffi.C.YGNodeStyleGetAlignContent(self.super)
end

--- Sets the align items attribute.
-- @param align ['auto', 'flex-start', 'center', 'flex-end', 'stretch', 'baseline', 'space-between', 'space-around']
-- @returns self
function Yoga:set_alignitems(align)
	align = Yoga.enums.align[align]
	ffi.C.YGNodeStyleSetAlignItems(self.super, align)
	return self
end

function Yoga:get_alignitems()
	return ffi.C.YGNodeStyleGetAlignItems(self.super)
end

--- Sets the align self attribute.
-- @param align ['auto', 'flex-start', 'center', 'flex-end', 'stretch', 'baseline', 'space-between', 'space-around']
-- @returns self
function Yoga:set_alignself(align)
	align = Yoga.enums.align[align]
	ffi.C.YGNodeStyleSetAlignSelf(self.super, align)
	return self
end

function Yoga:get_alignself()
	return ffi.C.YGNodeStyleGetAlignSelf(self.super)
end

--- Sets the position type attribute.
-- @param position ['static', 'relative', 'absolute']
-- @returns self
function Yoga:set_positiontype(position)
	position = Yoga.enums.position[position]
	ffi.C.YGNodeStyleSetPositionType(self.super, position);
	return self
end

function Yoga:get_positiontype()
	return ffi.C.YGNodeStyleGetPositionType(self.super)
end

function Yoga:set_aspectratio(ratio)
	ffi.C.YGNodeStyleSetAspectRatio(self.super, ratio)
	return self
end

function Yoga:get_aspectratio()
	return ffi.C.YGNodeStyleGetAspectRatio(self.super)
end

--- Sets the flex wrap attribute.
-- @param wrap ['no-wrap', 'wrap', 'wrap-reverse']
-- @returns self
function Yoga:set_flexwrap(wrap)
	wrap = Yoga.enums.wrap[wrap]
	ffi.C.YGNodeStyleSetFlexWrap(self.super, wrap);
	return self
end

function Yoga:get_flexwrap()
	return ffi.C.YGNodeStyleGetFlexWrap(self.super)
end

--- Sets the overflow attribute.
-- @param overflow ['visible', 'hidden', 'scroll']
-- @returns self
function Yoga:set_overflow(overflow)
	overflow = Yoga.enums.overflow[overflow]
	ffi.C.YGNodeStyleSetOverflow(self.super, overflow);
	return self
end

function Yoga:get_overflow()
	return ffi.C.YGNodeStyleGetOverflow(self.super)
end

function Yoga:set_flex(flex)
	ffi.C.YGNodeStyleSetFlex(self.super, flex);
	return self
end

function Yoga:get_flex()
	return ffi.C.YGNodeStyleGetFlex(self.super)
end

function Yoga:set_flexgrow(grow)
	ffi.C.YGNodeStyleSetFlexGrow(self.super, grow)
	return self
end

function Yoga:get_flexgrow()
	return ffi.C.YGNodeStyleGetFlexGrow(self.super)
end

function Yoga:set_flexshrink(shrink)
	ffi.C.YGNodeStyleSetFlexShrink(self.super, shrink)
	return self
end

function Yoga:get_flexshrink()
	return ffi.C.YGNodeStyleGetFlexShrink(self.super)
end

function Yoga:set_position(position, edge)
	if edge == nil then
		edge = ffi.C.YGEdgeAll
	else
		edge = Yoga.enums.edge[edge]
	end

	if string.sub(position, -1) == '%' then
		position = tonumber(string.sub(position, 0, -2))
		ffi.C.YGNodeStyleSetPositionPercent(self.super, edge, position)
	else
		ffi.C.YGNodeStyleSetPosition(self.super, edge, position)
	end

	return self
end

function Yoga:get_position(edge)
	if edge == nil then
		edge = ffi.C.YGEdgeAll
	else
		edge = Yoga.enums.edge[edge]
	end

	local obj = ffi.C.YGNodeStyleGetPosition(self.super, edge)

	if obj.unit == Yoga.enums.unit['percent'] then
		return obj.value .. '%'
	elseif obj.unit == Yoga.enums.unit['auto'] then
		return 'auto'
	end

	return obj.value
end

function Yoga:get_layoutleft()
	return ffi.C.YGNodeLayoutGetLeft(self.super)
end

function Yoga:get_layouttop()
	return ffi.C.YGNodeLayoutGetTop(self.super)
end

function Yoga:get_layoutright()
	return ffi.C.YGNodeLayoutGetRight(self.super)
end

function Yoga:get_layoutbottom()
	return ffi.C.YGNodeLayoutGetBottom(self.super)
end

function Yoga:get_layoutwidth()
	return ffi.C.YGNodeLayoutGetWidth(self.super)
end

function Yoga:get_layoutheight()
	return ffi.C.YGNodeLayoutGetHeight(self.super)
end

function Yoga:get_layout()
	return {
		top = self:get_layouttop(),
		left = self:get_layoutleft(),
		right = self:get_layoutright(),
		bottom = self:get_layoutbottom(),
		width = self:get_layoutwidth(),
		height = self:get_layoutheight(),
	}
end

function Yoga:get_hadoverflow()
	return ffi.C.YGNodeLayoutGetHadOverflow(self.super)
end

--- Sets the margin.
-- @param margin number|string The margin amount. Can be 'auto', a percentage like '75%' or a number.
-- @param edge='all' ['left', 'top', 'right', 'bottom', 'start', 'end', 'horizontal', 'vertical', 'all']
-- @returns self
function Yoga:set_margin(margin, edge)
	if edge == nil then
		edge = ffi.C.YGEdgeAll
	else
		edge = Yoga.enums.edge[edge]
	end

	if margin == 'auto' then
		ffi.C.YGNodeStyleSetMarginAuto(self.super, edge)
	elseif string.sub(margin, -1) == '%' then
		margin = tonumber(string.sub(margin, 0, -2))
		ffi.C.YGNodeStyleSetMarginPercent(self.super, edge, margin)
	else
		ffi.C.YGNodeStyleSetMargin(self.super, edge, margin)
	end

	return self
end

function Yoga:get_margin(edge)
	return ffi.C.YGNodeLayoutGetMargin(self.super, edge)
end

--- Sets the border.
-- @param border number The margin amount.
-- @param edge='all' ['left', 'top', 'right', 'bottom', 'start', 'end', 'horizontal', 'vertical', 'all']
-- @returns self
function Yoga:set_border(border, edge)
	if edge == nil then
		edge = ffi.C.YGEdgeAll
	else
		edge = Yoga.enums.edge[edge]
	end

	ffi.C.YGNodeStyleSetBorder(self.super, edge, border)
	return self
end

function Yoga:get_border(edge)
	return ffi.C.YGNodeLayoutGetBorder(self.super, edge)
end

--- Sets the padding.
-- @param padding number|string The padding amount. Can be a percentage like '75%' or a number.
-- @param edge='all' ['left', 'top', 'right', 'bottom', 'start', 'end', 'horizontal', 'vertical', 'all']
-- @returns self
function Yoga:set_padding(padding, edge)
	if edge == nil then
		edge = ffi.C.YGEdgeAll
	else
		edge = Yoga.enums.edge[edge]
	end

	if string.sub(padding, -1) == '%' then
		padding = tonumber(string.sub(padding, 0, -2))
		ffi.C.YGNodeStyleSetPaddingPercent(self.super, edge, padding)
	else
		ffi.C.YGNodeStyleSetPadding(self.super, edge, padding)
	end

	return self
end

function Yoga:get_padding(edge)
	return ffi.C.YGNodeLayoutGetPadding(self.super, edge)
end

function Yoga:insertchild(child, index)
	if index == nil then
		index = self:getchildcount()
	end

	ffi.C.YGNodeInsertChild(self.super, child.super, index)
	return self
end

function Yoga:removechild(child)
	ffi.C.YGNodeRemoveChild(self.super, child.super)
	return self
end

function Yoga:removechildren()
	ffi.C.YGNodeRemoveAllChildren(self.super)
	return self
end

function Yoga:getchild(index)
	return Yoga.new(ffi.C.YGNodeGetChild(self.super, index))
end

function Yoga:getowner()
	return Yoga.new(ffi.C.YGNodeGetOwner(self.super))
end

function Yoga:getparent()
	return Yoga.new(ffi.C.YGNodeGetParent(self.super))
end

function Yoga:getchildcount()
	return ffi.C.YGNodeGetChildCount(self.super)
end

function Yoga:calculate()
	ffi.C.YGNodeCalculateLayout(self.super, self:get_width(), self:get_height(), ffi.C.YGDirectionLTR)
	return self
end

return Yoga
