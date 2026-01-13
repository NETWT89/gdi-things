local u=game:GetService("UserInputService")
local p=game:GetService("Players").LocalPlayer
local c=nil
local r=false
local g=Instance.new("ScreenGui")
g.Name="uhh"
g.ResetOnSpawn=false
g.Parent=p:WaitForChild("PlayerGui")
local b=Instance.new("TextButton")
b.Parent=g
b.Text=""
b.AutoButtonColor=true
b.BackgroundColor3=Color3.fromRGB(255,255,255)
b.BackgroundTransparency=0.3
b.BorderSizePixel=0
b.AnchorPoint=Vector2.new(0.5,0.5)
local function sz()
	local v=workspace.CurrentCamera.ViewportSize
	b.Size=UDim2.fromOffset(v.X*0.18,v.X*0.18)
	b.Position=UDim2.fromOffset(v.X*0.8,v.Y*0.75)
	b.UICorner=Instance.new("UICorner",b)
	b.UICorner.CornerRadius=UDim.new(1,0)
end
sz()
workspace.CurrentCamera:GetPropertyChangedSignal("ViewportSize"):Connect(sz)
local function spd(x)
	if c then c.WalkSpeed=x end
end
local function hook()
	c=p.Character and p.Character:FindFirstChildOfClass("Humanoid")
	if c then spd(24) end
end
p.CharacterAdded:Connect(function()
	task.wait(0.2)
	hook()
end)
hook()
u.InputBegan:Connect(function(i,gp)
	if gp then return end
	if i.KeyCode==Enum.KeyCode.LeftShift then
		r=true
		spd(30)
	end
end)
u.InputEnded:Connect(function(i)
	if i.KeyCode==Enum.KeyCode.LeftShift then
		r=false
		spd(24)
	end
end)
b.MouseButton1Down:Connect(function()
	r=true
	spd(30)
end)
b.MouseButton1Up:Connect(function()
	r=false
	spd(24)
end)
b.TouchEnded:Connect(function()
	r=false
	spd(24)
end)
