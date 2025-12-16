import { useState, useEffect } from 'react';
import './TaskForm.css';

const TaskForm = ({ task, onSave, onCancel }) => {
	const [title, setTitle] = useState('');
	const [description, setDescription] = useState('');
	const [dueDate, setDueDate] = useState('');
	const [createdDate, setCreatedDate] = useState('');
	const [priority, setPriority] = useState('medium');

	useEffect(() => {
		if (task) {
			setTitle(task.title || '');
			setDescription(task.description || '');
			setDueDate(task.due_date || '');
			setCreatedDate(task.created_at ? task.created_at.split(' ')[0] : '');
			setPriority(task.priority || 'medium');
		}
	}, [task]);

	const handleSubmit = (e) => {
		e.preventDefault();
		if (!title.trim()) {
			alert('Название задачи обязательно');
			return;
		}

		onSave({
			title: title.trim(),
			description: description.trim(),
			due_date: dueDate || null,
			created_at: createdDate || null,
			priority: priority,
		});
	};

	return (
		<div className='modal-overlay' onClick={onCancel}>
			<div className='modal-content' onClick={(e) => e.stopPropagation()}>
				<h2>{task ? 'Редактировать задачу' : 'Добавить новую задачу'}</h2>

				<form onSubmit={handleSubmit}>
					<div className='form-group'>
						<label htmlFor='title'>Название *</label>
						<input
							type='text'
							id='title'
							value={title}
							onChange={(e) => setTitle(e.target.value)}
							required
							placeholder='Введите название задачи'
						/>
					</div>

					<div className='form-group'>
						<label htmlFor='description'>Описание</label>
						<textarea
							id='description'
							value={description}
							onChange={(e) => setDescription(e.target.value)}
							rows='4'
							placeholder='Введите описание задачи'
						/>
					</div>

					<div className='form-group'>
						<label htmlFor='createdDate'>Дата создания</label>
						<input
							type='date'
							id='createdDate'
							value={createdDate}
							onChange={(e) => setCreatedDate(e.target.value)}
						/>
					</div>

					<div className='form-group'>
						<label htmlFor='dueDate'>Срок выполнения</label>
						<input
							type='date'
							id='dueDate'
							value={dueDate}
							onChange={(e) => setDueDate(e.target.value)}
						/>
					</div>

					<div className='form-group'>
						<label htmlFor='priority'>Приоритет</label>
						<select
							id='priority'
							value={priority}
							onChange={(e) => setPriority(e.target.value)}
						>
							<option value='low'>Низкий</option>
							<option value='medium'>Средний</option>
							<option value='high'>Высокий</option>
						</select>
					</div>

					<div className='form-actions'>
						<button type='button' onClick={onCancel} className='btn-cancel'>
							Отмена
						</button>
						<button type='submit' className='btn-save'>
							{task ? 'Сохранить' : 'Создать'}
						</button>
					</div>
				</form>
			</div>
		</div>
	);
};

export default TaskForm;
