import axios from 'axios';

let API_BASE_URL = import.meta.env.VITE_API_URL || '/api';

// Если URL не начинается с http/https, но содержит точку (домен), добавляем https://
if (!API_BASE_URL.startsWith('http') && API_BASE_URL.includes('.')) {
	API_BASE_URL = 'https://' + API_BASE_URL;
}

// Если указан полный URL без /api, добавляем его
if (API_BASE_URL.startsWith('http') && !API_BASE_URL.endsWith('/api')) {
	if (!API_BASE_URL.endsWith('/')) {
		API_BASE_URL += '/api';
	} else {
		API_BASE_URL += 'api';
	}
}

const api = axios.create({
	baseURL: API_BASE_URL,
	headers: {
		'Content-Type': 'application/json; charset=utf-8',
	},
});

const containsNonASCII = (str) => {
	return /[^\x00-\x7F]/.test(str);
};

api.interceptors.request.use(
	(config) => {
		const token = localStorage.getItem('token');
		if (token) {
			if (containsNonASCII(token)) {
				console.warn(
					'Токен содержит не-ASCII символы, требуется повторный вход'
				);
				localStorage.removeItem('token');
				localStorage.removeItem('user');
				window.location.href = '/';
				return Promise.reject(new Error('Invalid token encoding'));
			}
			config.headers.Authorization = `Bearer ${token}`;
		}
		return config;
	},
	(error) => {
		return Promise.reject(error);
	}
);

api.interceptors.response.use(
	(response) => response,
	(error) => {
		if (error.response?.status === 401) {
			localStorage.removeItem('token');
			localStorage.removeItem('user');
			window.location.href = '/';
		}
		return Promise.reject(error);
	}
);

export const authAPI = {
	register: async (username, password) => {
		const response = await api.post('/auth/register', { username, password });
		return response.data;
	},

	login: async (username, password) => {
		const response = await api.post('/auth/login', { username, password });
		if (response.data.token) {
			localStorage.setItem('token', response.data.token);
			localStorage.setItem(
				'user',
				JSON.stringify({
					id: response.data.user_id,
					username: response.data.username,
				})
			);
		}
		return response.data;
	},

	logout: () => {
		localStorage.removeItem('token');
		localStorage.removeItem('user');
	},

	getCurrentUser: () => {
		const userStr = localStorage.getItem('user');
		return userStr ? JSON.parse(userStr) : null;
	},

	isAuthenticated: () => {
		return !!localStorage.getItem('token');
	},
};

export const tasksAPI = {
	getAll: async () => {
		const response = await api.get('/tasks');
		return response.data;
	},

	create: async (task) => {
		const response = await api.post('/tasks', task);
		return response.data;
	},

	update: async (id, task) => {
		const response = await api.put(`/tasks/${id}`, task);
		return response.data;
	},

	delete: async (id) => {
		const response = await api.delete(`/tasks/${id}`);
		return response.data;
	},
};

export default api;
