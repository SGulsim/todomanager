export const formatDate = (dateString) => {
	if (!dateString) return '';

	const date = new Date(dateString);
	if (isNaN(date.getTime())) return '';

	const day = String(date.getDate()).padStart(2, '0');
	const month = String(date.getMonth() + 1).padStart(2, '0');
	const year = date.getFullYear();

	return `${day}.${month}.${year}`;
};

export const isOverdue = (dueDateString, status, createdDateString = null) => {
	if (!dueDateString || status === 'completed') return false;

	const dueDate = new Date(dueDateString);
	const today = new Date();
	today.setHours(0, 0, 0, 0);
	dueDate.setHours(0, 0, 0, 0);

	// Если указана дата создания и она в прошлом, но дедлайн еще не прошел - задача не просрочена
	if (createdDateString) {
		const createdDate = new Date(createdDateString);
		createdDate.setHours(0, 0, 0, 0);
		
		// Если дата создания в прошлом, но дедлайн еще не подошел - задача не просрочена
		if (createdDate < today && dueDate >= today) {
			return false;
		}
	}

	// Если дедлайн прошел - задача просрочена
	return dueDate < today;
};

export const isToday = (dateString) => {
	if (!dateString) return false;

	const date = new Date(dateString);
	const today = new Date();

	return (
		date.getDate() === today.getDate() &&
		date.getMonth() === today.getMonth() &&
		date.getFullYear() === today.getFullYear()
	);
};

export const isThisWeek = (dateString) => {
	if (!dateString) return false;

	const date = new Date(dateString);
	const today = new Date();
	const weekStart = new Date(today);
	weekStart.setDate(today.getDate() - today.getDay());
	weekStart.setHours(0, 0, 0, 0);
	const weekEnd = new Date(weekStart);
	weekEnd.setDate(weekStart.getDate() + 6);
	weekEnd.setHours(23, 59, 59, 999);

	return date >= weekStart && date <= weekEnd;
};
